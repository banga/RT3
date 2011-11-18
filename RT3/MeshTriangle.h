#pragma once
#include "Object3d.h"

class MeshTriangle:public Object3d
{
public:
	Vector a, ab, ac, n1, n12, n13;
	float u1, u12, u13, v1, v12, v13;

	MeshTriangle(Vector a, Vector b, Vector c, Vector n1, Vector n2, Vector n3, Material* material);
	MeshTriangle(Vector a, Vector b, Vector c, float u[3], float v[3], Material* material);
	MeshTriangle(Vector a, Vector b, Vector c, Vector n1, Vector n2, Vector n3, float u[3], float v[3], Material* material);
	bool Intersect(Ray& ray, Intersection& intersection);
	bool IntersectP(Ray& ray);
	void GetBounds(Vector&, Vector&) ;

public:
	~MeshTriangle(void);
};
