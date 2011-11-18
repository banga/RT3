#include "Object3d.h"

int Object3d::count = 0;

Object3d::Object3d(Material* m):material(m)
{
	transform = NULL;
	count++;
}

bool Object3d::Intersect(Ray&, Intersection& i)
{
	throw "Object3d base class Intersect() called";
}

bool Object3d::IntersectP(Ray&)
{
	throw "Object3d base class Intersect() called";
}

void Object3d::GetBounds(Vector&, Vector&)
{
	throw "Object3d base class GetBounds() called";
}

void Object3d::applyTransform(Transform* t)
{
	if(!t) return;
	if(transform)
		*transform = (*t) * (*transform);
	else
		transform = new Transform(*t);
}

Object3d::~Object3d(void)
{
	if(transform)
		delete transform;
	count--;
}