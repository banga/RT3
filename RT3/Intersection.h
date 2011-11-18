#pragma once
#include "Defines.h"
#include "Vector.h"

class Object3d;

class Intersection
{
public:
	Object3d* object;
	Vector point, normal;
	float u, v;

	Intersection()
	{
		object = NULL;
		u = v = 0;
	}

	Intersection(const Intersection& i)
	{
		set(i.object,i.point,i.normal,i.u,i.v);
	}

	Intersection(Object3d *o, Vector p, Vector n, float _u, float _v)
	{
		set(o,p,n,_u,_v);
	}

	void set(Object3d *o, Vector p, Vector n, float _u, float _v)
	{
		object = o;
		point = p;
		normal = n;
		u = _u;
		v = _v;
	}
};
