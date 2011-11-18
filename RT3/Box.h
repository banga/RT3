#pragma once
#include "Object3d.h"

class Box:public Object3d
{
	static const float BOX_EPSILON;

public:
	Box(Vector corner1, Vector corner2, Material* material);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector &min, Vector &max) ;

public:
	~Box(void);
};