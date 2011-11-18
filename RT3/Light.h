#pragma once
#include "Defines.h"
#include "Vector.h"
#include "Spectrum.h"

class Light
{
public:
	Spectrum Le;

	Light(Spectrum c);
	virtual Vector origin();
public:
	~Light(void);
};
