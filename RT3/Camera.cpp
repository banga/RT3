
#include "Camera.h"

/// <param name
Camera::Camera(Vector o, Vector u, Vector r):
up(u), 
right(r),
origin(o)
{
	forward = Vector::crossProduct(up, right).normal();
}

Ray Camera::getRayAt(float x, float y)
{
	throw("Camera base class called");
}

void Camera::lookAt(Vector p)
{
	forward = (p - origin).normal();
	float width = right.magnitude();
	right = Vector::crossProduct(forward, up).normal() * width;	
}

// x = [0,imageWidth-1]
// y = [0,imageHeight-1]
Camera::~Camera(void)
{
}