
#include "Plane.h"

Plane::Plane(Vector p, Vector n, Material* m):Object3d(m)
{
	n = n.normal();
	float thetay = ACOS(n.z) * M_180_PI;
	float thetaz = ATAN(n.y,n.x) * M_180_PI;
	transform = new Transform(Transform::Translate(p.x,p.y,p.z) * Transform::RotateZ(thetaz) * Transform::RotateY(thetay));
}

bool Plane::Intersect(Ray& r, Intersection& i)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	if(fabs(ray.O.z) < EPSILON || fabs(ray.D.z) < EPSILON)
		return false;

	float t = -ray.O.z / ray.D.z;

	if(r.trunc(t))
	{
		Vector P = ray.getPointAt(t);
		float u = fmod(P.x,1.f), v = fmod(P.y,1.f);
		if(u < 0.f) u = 1+u;
		if(v < 0.f) v = 1+v;
		i.set(this,P,Vector(0,0,1),u,v);
		return true;
	}

	return false;
}

bool Plane::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	if(fabs(ray.O.z) < EPSILON || fabs(ray.D.z) < EPSILON)
		return false;

	float t = -ray.O.z / ray.D.z;

	return r.trunc(t);
}

Plane::~Plane(void)
{
}
