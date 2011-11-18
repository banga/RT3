
#include "TriangleLight.h"
#include <stdlib.h>

TriangleLight::TriangleLight(Vector _a, Vector _b, Vector _c, Spectrum c):a(_a), ab(_b-_a), ac(_c-_a), Light(c)
{
}

Vector TriangleLight::origin()
{
	float u, v;
	do {
		u = FRAND(); v = FRAND();
	} while(u + v > 1.0f);
	return (a + ab * u + ac * v);
}

TriangleLight::~TriangleLight(void)
{
}
