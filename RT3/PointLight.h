#pragma once
#include "Defines.h"
#include "Vector.h"
#include "Light.h"

class PointLight:public Light
{
public:
	Vector _origin;

	PointLight(Vector o, Spectrum Le);
	Vector origin() ;
public:
	~PointLight(void);
};
