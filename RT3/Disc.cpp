#include "Disc.h"

Disc::Disc(Vector p, Vector n, float r, Material* m):Object3d(m)
{
	n = n.normal();
	float thetay = ACOS(n.z) * M_180_PI;
	float thetaz = ATAN(n.y,n.x) * M_180_PI;
	transform = new Transform(Transform::Translate(p.x,p.y,p.z) * Transform::RotateZ(thetaz) * Transform::RotateY(thetay) * Transform::Scale(r,r,r));
}

void Disc::GetBounds(Vector &min, Vector &max)
{
	min = Vector(-1,-1,0);
	max = Vector( 1, 1,0);
	if(transform) transform->TransformBounds(min,max);
}

bool Disc::Intersect(Ray& r, Intersection& i)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	if(ray.D.z == 0) return false;

	float t = -ray.O.z / ray.D.z;

	if(r.inRange(t))
	{
		Vector P = ray.getPointAt(t);
		float u = Vector::dotProduct(P,P);

		if(u <= 1.f)
		{
			r.trunc(t);
			u = SQRT(u);
			float v = 0.5f + ATAN(P.y,P.x) * M_1_2PI;
			i.set(this,P,Vector(0,0,1),u,v);
			return true;
		}
	}

	return false;
}

bool Disc::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	if(ray.D.z == 0) return false;

	float t = -ray.O.z / ray.D.z;

	if(r.inRange(t))
	{
		Vector P = ray.getPointAt(t);
		if (Vector::dotProduct(P,P) <= 1)
		{
			r.trunc(t);
			return true;
		}
	}

	return false;
}

Disc::~Disc(void)
{
}
