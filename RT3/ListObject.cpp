
#include "ListObject.h"

ListObject::ListObject(const vector<Object3d*>& o):Object3d(NULL)
{
	vector<Object3d*>::const_iterator it = o.begin();
	int k = 0;
	Vector omin, omax;

	nObjects = o.size();
	objects = new Object3d*[nObjects];
	while(it < o.end()) {
		objects[k] = *it;
		(*it)->GetBounds(omin, omax);

		if(k == 0) {
			min = omin;
			max = omax;
		} else {
			min.x = min.x < omin.x ? min.x : omin.x;
			min.y = min.y < omin.y ? min.y : omin.y;
			min.z = min.z < omin.z ? min.z : omin.z;
			max.x = max.x > omax.x ? max.x : omax.x;
			max.y = max.y > omax.y ? max.y : omax.y;
			max.z = max.z > omax.z ? max.z : omax.z;
		}
		k++;
		it++;
	}
}

void ListObject::GetBounds(Vector &_min, Vector &_max)
{
	_min = min;
	_max = max;
	if(transform) transform->TransformBounds(min,max);
}

bool ListObject::Intersect(Ray& r, Intersection& intersection)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	bool intersected = false;
	Object3d *o;
	for(int i = 0; i < nObjects; i++) {
		o = objects[i];
		if(o->Intersect(ray,intersection))
			intersected = true;
	}
	return intersected;
}


bool ListObject::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	Object3d *o;
	for(int i = 0; i < nObjects; i++) {
		o = objects[i];
		if(o->IntersectP(ray))
			return true;
	}
	return false;
}

ListObject::~ListObject(void)
{
	if(objects) {
		for(int i = 0; i < nObjects; i++)
			delete objects[i];
		delete[] objects;
	}
}
