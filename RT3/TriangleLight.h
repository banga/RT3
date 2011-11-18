#pragma once
#include "Defines.h"
#include "Vector.h"
#include "Light.h"

class TriangleLight:public Light
{
public:
	Vector a, ab, ac;

	TriangleLight(Vector _a, Vector _b, Vector _c, Spectrum c);
	Vector origin() ;
public:
	~TriangleLight(void);
};
