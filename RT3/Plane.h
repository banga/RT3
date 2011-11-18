#pragma once
#include "Object3d.h"

class Plane:public Object3d
{
public:
	Plane(Vector point, Vector normal, Material* material);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);

public:
	~Plane(void);
};
