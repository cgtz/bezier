
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

#include "Parser.h"

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
bool mode; // true is adaptive, false is uniform



//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));



  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {

        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);

        setPixel(i,j, 1.0, 0.0, 0.0);

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }


    }
  }


  glEnd();
}

//Return point and derivative
pair<vec3, vec3> bezcurveinterpolate(BezCurve curve, double u)
{
	vec3 A = curve.p0 * (1.0-u) + curve.p1 * u;
	vec3 B = curve.p1 * (1.0-u) + curve.p2 * u;
	vec3 C = curve.p2 * (1.0-u) + curve.p3 * u;

	vec3 D = A * (1.0-u) + B * u;
	vec3 E = B * (1.0-u) + C * u;

	return pair<vec3, vec3>(D * (1.0-u) + E * u, 3*(E - D));
}

//Return surface point and normal
pair<vec3,vec3> bezpatchinterpolate(BezPatch patch, double u, double v)
{
	//Control points in v
	BezCurve vcurve;
	vcurve.p0 = bezcurveinterpolate(patch.c0, u).first;
	vcurve.p1 = bezcurveinterpolate(patch.c1, u).first;
	vcurve.p2 = bezcurveinterpolate(patch.c2, u).first;
	vcurve.p3 = bezcurveinterpolate(patch.c3, u).first;

	//Control points in u
	BezCurve ucurve;
	ucurve.p0 = bezcurveinterpolate(patch.c0, v).first;
	ucurve.p1 = bezcurveinterpolate(patch.c1, v).first;
	ucurve.p2 = bezcurveinterpolate(patch.c2, v).first;
	ucurve.p3 = bezcurveinterpolate(patch.c3, v).first;

	pair<vec3,vec3> pdV = bezcurveinterpolate(vcurve, v);
	pair<vec3,vec3> pdU = bezcurveinterpolate(ucurve, u);

	if (pdV.first != pdU.first) cout << "Points don't match!" << endl;

	return pair<vec3,vec3>(pdV.first, (pdU.second^pdV.second).normalize());

}

void uniformsubdivision(BezPatch patch, double step)
{
	int numdiv = 1.01 / step;
	for (int iu=0; iu<numdiv; iu++){
		double u = iu * step;
		for(int iv=0; iv<numdiv; iv++){
			double v = iv * step;
			pair<vec3,vec3> pn = bezpatchinterpolate(patch, u, v); //do x4 ?
			//Don't store. Draw write(pun intended) away!
			//glNormal * 4
			//glVertex * 4
		}
	}
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

	if (argc > 1){
		mode = (argc == 4);
		vector<BezPatch> patches(1);
		loadScene(argv[1], patches, atof(argv[2]), atof(argv[2]));
		//cout << patches[0].c1.p0  << " " << patches[0].c2.p1  << " " << patches[0].c3.p2  << " " << patches[0].u << endl;
		cout << "Done parsing." << endl;
	}
	else {
		cout << "Unable to find file. No arguments given." << endl;
	}
	
	
	//This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  //PSUEDOCODE 
  //if (mode){
	 // //for each patch i in all patches
	 // glBegin(GL_TRIANGLES);
		//adaptivesubdivide(patch i, step)
	 // glEnd();
  //
  //}
  //else{
	 // //for each patch i in all patches
	 // glBegin(GL_TRIANGLES);
		//uniformsubdivision(patch i, step)
	 // glEnd();
  //}



  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}








