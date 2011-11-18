#pragma once
#include "Defines.h"
#include "Vector.h"
#include "Intersection.h"

const float RAY_EPSILON = EPSILON;

class Ray
{
public:
	Vector O, D;
	float mint, maxt;

	Ray(void)
	{
		mint = RAY_EPSILON;
		maxt = INFINITY;
	}

	Ray(Vector _O, Vector _D)
	{
		O = _O;
		D = _D;
		mint = RAY_EPSILON;
		maxt = INFINITY;
	}

	Ray(Vector _O, Vector _D, float _mint, float _maxt)
	{
		O = _O;
		D = _D;
		mint = _mint;
		maxt = _maxt;
	}

	Ray(const Ray& r)
	{
		O = r.O;
		D = r.D;
		mint = r.mint;
		maxt = r.maxt;
	}

	Vector getPointAt(float t) const
	{
		return (O + D * t);
	}

	bool inRange(float t)
	{
		if ((t > mint) && (t < maxt))
		{
			return true;
		}
		return false;
	}

	bool trunc(float t)
	{
		if(inRange(t))
		{
			maxt = t;
			return true;
		}
		return false;
	}
};
