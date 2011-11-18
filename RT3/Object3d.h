#pragma once
#include "Defines.h"
#include "Vector.h"
#include "Transform.h"
#include "Material.h"

class Object3d
{
public:
	static int count;

	Material *material;
	Transform *transform;

	Object3d(Material* material);
	virtual bool Intersect(Ray& ray, Intersection& intersection);
	virtual bool IntersectP(Ray& ray);
	virtual void GetBounds(Vector&, Vector&);
	void applyTransform(Transform* t);
	~Object3d(void);
};
