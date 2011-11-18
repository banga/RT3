#include "Sphere.h"

Sphere::Sphere(Vector o, float r, Material* m):Object3d(m)
{
	transform = new Transform(Transform::Translate(o.x,o.y,o.z) * Transform::Scale(r,r,r));
}

void Sphere::GetBounds(Vector &min, Vector &max)
{
	min = Vector(-1,-1,-1);
	max = Vector(1,1,1);
	if(transform) transform->TransformBounds(min,max);
}

void SphereUV(Vector P, float& u, float& v)
{
	u = 0.5f + ATAN(P.y,P.x) * M_1_2PI;
	v = ACOS(P.z) * M_1_PI;
}

bool Sphere::Intersect(Ray& r, Intersection& i)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	float DD = Vector::dotProduct(ray.D,ray.D);
	float DO = Vector::dotProduct(ray.O,ray.D);
	float OO = Vector::dotProduct(ray.O,ray.O);

	if(DD == 0.f)
	{
		float t = (1 - OO) / (2 * DO);
		if(r.trunc(t))
		{
			Vector P = ray.getPointAt(t);
			float u, v;
			SphereUV(P,u,v);
			i.set(this,P,P,u,v);
			return true;
		}
	}
	else
	{
		float delta = DO*DO - DD * (OO - 1);

		if(delta < 0.f) return false;

		if(delta == 0.f)
		{
			float t = -DO / DD;
			if(r.trunc(t))
			{
				Vector P = ray.getPointAt(t);
				float u, v;
				SphereUV(P,u,v);
				i.set(this,P,P,u,v);
				return true;
			}
		}
		else
		{
			delta = SQRT(delta);
			float t1 = -(DO + delta) / DD;
			if(r.trunc(t1))
			{
				Vector P = ray.getPointAt(t1);
				float u, v;
				SphereUV(P,u,v);
				i.set(this,P,P,u,v);
				return true;
			}
			float t2 = (-DO + delta) / DD;
			if(r.trunc(t2))
			{
				Vector P = ray.getPointAt(t2);
				float u, v;
				SphereUV(P,u,v);
				i.set(this,P,P,u,v);
				return true;
			}
		}
	}
	return false;
}

bool Sphere::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	float DD = Vector::dotProduct(ray.D,ray.D);
	float DO = Vector::dotProduct(ray.O,ray.D);
	float OO_m1 = Vector::dotProduct(ray.O,ray.O) - 1.f;

	if(fabs(OO_m1) < EPSILON) return false;

	if(DD == 0.f)
	{
		float t = -OO_m1 / (2 * DO);
		return r.trunc(t);
	}
	else
	{
		float delta = DO*DO - DD * (OO_m1);

		if(delta < 0.f) return false;

		if(delta == 0.f)
		{
			float t = -DO / DD;
			return r.trunc(t);
		}
		else
		{
			delta = SQRT(delta);
			float t1 = -(DO + delta) / DD;
			if (r.trunc(t1))
				return true;
			float t2 = (-DO + delta) / DD;
			if(r.trunc(t2))
				return true;
		}
	}
	return false;
}
