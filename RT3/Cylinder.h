#pragma once
#include "Object3d.h"

class Cylinder:public Object3d
{
public:
	Cylinder(Vector center1, Vector center2, float radius, Material* material);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector&, Vector&) ;
};
