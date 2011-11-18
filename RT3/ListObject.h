#pragma once
#include "Object3d.h"

class ListObject:public Object3d
{
public:
	// Bounding box
	Vector min, max;
	Object3d** objects;
	int nObjects;

	ListObject(const vector<Object3d*>& objects);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector &min, Vector &max) ;
public:
	~ListObject(void);
};
