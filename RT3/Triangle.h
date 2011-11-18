#pragma once
#include "Object3d.h"

class Triangle:public Object3d
{
public:
	Vector a, ab, ac, normal;

	Triangle(Vector a, Vector b, Vector c, Material* material);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector&, Vector&) ;

public:
	~Triangle(void);
};
