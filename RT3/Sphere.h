#pragma once
#include "Object3d.h"

class Sphere:public Object3d
{
public:
	Sphere(Vector center, float radius, Material* material);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector&, Vector&) ;
};
