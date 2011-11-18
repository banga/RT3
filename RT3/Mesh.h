#pragma once
#include "KDTreeObject.h"
#include "Triangle.h"
#include "MeshTriangle.h"
#include "ListObject.h"
#include <map>
#include <string>

using namespace std;

class Mesh:public Object3d
{
public:
	// Bounding box
	Object3d* mesh;
	//array<Triangle^>^ triangles;
	//int count;

	Mesh(string fname, Material* m);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector &min, Vector &max) ;
public:
	~Mesh(void);
};
