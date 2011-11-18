
#include "PointLight.h"

PointLight::PointLight(Vector o, Spectrum c):_origin(o), Light(c)
{
}

Vector PointLight::origin()
{
	return _origin;
}

PointLight::~PointLight(void)
{
}
