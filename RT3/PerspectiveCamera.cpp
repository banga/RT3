
#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(Vector o, Vector u, Vector r, float a):Camera(o,u,r)
{
	imageCorner = origin - (up * 0.5f) - (right * 0.5f);

	if(a == 0)
		throw("Perspective camera angle = 0");

	a = DEG_2_RAD(a);
	float t = tanf(a * 0.5f);
	distance = Vector::dotProduct(u,u) / (t*t) - Vector::dotProduct(r,r);

	if(distance < 0)
		throw("Perspective camera angle impossible");

	distance = 0.5f * SQRT(distance);
	center = origin - forward * distance;
}

Ray PerspectiveCamera::getRayAt(float x, float y)
{
	Vector point = imageCorner + up * y + right * x;
	return Ray(point, (point-center).normal());
}

void PerspectiveCamera::lookAt(Vector p)
{
	forward = (p - origin).normal();
	float width = right.magnitude();
	float height = up.magnitude();
	float u_dot_f = Vector::dotProduct(up, forward);

	if(u_dot_f == height)
		throw("Up and Forward cannot be equal in PerspectiveCamera");

	// To keep 'up' perpendicular
	up = (up - forward * u_dot_f) * powf(1 - powf(u_dot_f/height, 2), -0.5f);
	right = Vector::crossProduct(forward, up).normal() * width;

	center = origin - forward * distance;
	imageCorner = origin - (up * 0.5f) - (right * 0.5f);
}

PerspectiveCamera::~PerspectiveCamera(void)
{
}
