#include "BezCurve.h"


BezCurve::BezCurve(vec3 p0, vec3 p1, vec3 p2, vec3 p3, double u)
{
	this->p0 = p0;
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->u = u;
}

