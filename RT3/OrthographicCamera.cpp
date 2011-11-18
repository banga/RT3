
#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(Vector o, Vector u, Vector r):Camera(o,u,r)
{
	imageCorner = origin - up * 0.5f - right * 0.5f;
}

Ray OrthographicCamera::getRayAt(float x, float y)
{
	return Ray(imageCorner + (up * y) + (right * x), forward);
}

OrthographicCamera::~OrthographicCamera(void)
{
}
