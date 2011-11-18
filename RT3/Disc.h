#pragma once
#include "Object3d.h"
#include "Transform.h"

class Disc:public Object3d
{
public:
	Disc(Vector center, Vector normal, float radius, Material* material);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector &min, Vector &max) ;

public:
	~Disc(void);
};
