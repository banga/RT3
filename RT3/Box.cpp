#include "Box.h"

const float Box::BOX_EPSILON = EPSILON;

Box::Box(Vector p1, Vector p2, Material* m):Object3d(m)
{
	if(p1.x > p2.x) SWAP(p1.x,p2.x);
	if(p1.y > p2.y) SWAP(p1.y,p2.y);
	if(p1.z > p2.z) SWAP(p1.z,p2.z);

	transform = new Transform(Transform::Translate(p1.x,p1.y,p1.z) * Transform::Scale(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z));
}

void Box::GetBounds(Vector &min, Vector &max)
{
	min = Vector(0,0,0);
	max = Vector(1,1,1);
	if(transform) transform->TransformBounds(min,max);
}

bool Box::Intersect(Ray& r, Intersection& i)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);
	float t0 = ray.mint, t1 = ray.maxt;

	for(int axis = 0; axis < 3; axis++) {
		float Ox = ray.O[axis], Dx = ray.D[axis];
		float invD = 1.0f / Dx;
		float tn = -Ox * invD;
		float tf = invD + tn;

		if(tn > tf) {
			float t = tn;
			tn = tf;
			tf = t;
		}
		if(tn > t0) t0 = tn;
		if(tf < t1) t1 = tf;
		if(t0 > t1) return false;
	}

	if(r.trunc(t0))
	{
		Vector d1 = ray.getPointAt(t0), d2 = d1-Vector(1,1,1);
		Vector point = d1, normal;
		float u, v;

		if(fabs(d1.x) < BOX_EPSILON)
		{
			normal.x = -1;
			u = 0.25f * (1 - point.z);
			v = 1/3.f * (1 + point.y);
		}
		else if(fabs(d2.x) < BOX_EPSILON)
		{
			normal.x = 1;
			u = 0.25f * (2 + point.z);
			v = 1/3.f * (1 + point.y);
		}
		else if(fabs(d1.y) < BOX_EPSILON)
		{
			normal.y = -1;
			u = 0.25f * (1 + point.x);
			v = 1/3.f * (1 - point.z);
		}
		else if(fabs(d2.y) < BOX_EPSILON)
		{
			normal.y = 1;
			u = 0.25f * (1 + point.x);
			v = 1/3.f * (2 + point.z);
		}
		else if(fabs(d1.z) < BOX_EPSILON)
		{
			normal.z = -1;
			u = 0.25f * (1 + point.x);
			v = 1/3.f * (1 + point.y);
		}
		else if(fabs(d2.z) < BOX_EPSILON)
		{
			normal.z = 1;
			u = 0.25f * (4 - point.x);
			v = 1/3.f * (1 + point.y);
		}

		i.set(this,point,normal,u,v);
		return true;
	}

	if(r.trunc(t1))
	{
		Vector d1 = ray.getPointAt(t1), d2 = d1-Vector(1,1,1);
		Vector point = d1, normal;
		float u, v;

		if(fabs(d1.x) < BOX_EPSILON)
		{
			normal.x = -1;
			u = 0.25f * (1 - point.z);
			v = 1/3.f * (1 + point.y);
		}
		else if(fabs(d2.x) < BOX_EPSILON)
		{
			normal.x = 1;
			u = 0.25f * (2 + point.z);
			v = 1/3.f * (1 + point.y);
		}
		else if(fabs(d1.y) < BOX_EPSILON)
		{
			normal.y = -1;
			u = 0.25f * (1 + point.x);
			v = 1/3.f * (1 - point.z);
		}
		else if(fabs(d2.y) < BOX_EPSILON)
		{
			normal.y = 1;
			u = 0.25f * (1 + point.x);
			v = 1/3.f * (2 + point.z);
		}
		else if(fabs(d1.z) < BOX_EPSILON)
		{
			normal.z = -1;
			u = 0.25f * (1 + point.x);
			v = 1/3.f * (1 + point.y);
		}
		else if(fabs(d2.z) < BOX_EPSILON)
		{
			normal.z = 1;
			u = 0.25f * (4 - point.x);
			v = 1/3.f * (1 + point.y);
		}

		i.set(this,point,normal,u,v);
		return true;
	}

	return false;
}

bool Box::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);
	float t0 = ray.mint, t1 = ray.maxt;

	for(int axis = 0; axis < 3; axis++) {
		float Ox = ray.O[axis], Dx = ray.D[axis];
		float invD = 1.0f / Dx;
		float tn = -Ox * invD;
		float tf = (1 - Ox) * invD;

		if(tn > tf) {
			float t = tn;
			tn = tf;
			tf = t;
		}
		if(tn > t0) t0 = tn;
		if(tf < t1) t1 = tf;
		if(t0 > t1) return false;
	}

	bool intersection;
	if(r.trunc(t0)) intersection = true;
	if(r.trunc(t1)) intersection = true;
	return intersection;
}

Box::~Box(void)
{
}
