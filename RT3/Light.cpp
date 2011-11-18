
#include "Light.h"

Light::Light(Spectrum c):Le(c)
{
}

Vector Light::origin()
{
	throw "Light base class origin() called";
}

Light::~Light(void)
{
}
