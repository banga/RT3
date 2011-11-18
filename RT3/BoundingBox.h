#pragma once
#include "Defines.h"
#include "Ray.h"

class BoundingBox
{
public:
	Vector min, max;

	BoundingBox();
	BoundingBox(const BoundingBox& bbox);
	BoundingBox(Vector &min, Vector &max);

	void Union(const Vector &point);
	bool Intersect(const Ray& r);
	BoundingBox operator = (BoundingBox& bb);
};
