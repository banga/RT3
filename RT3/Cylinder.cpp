#include "Cylinder.h"

Cylinder::Cylinder(Vector c1, Vector c2, float r, Material* m):Object3d(m)
{
	Vector n = (c2-c1);
	float h = n.magnitude();
	n = n * (1.f/h);
	float thetay = ACOS(n.z) * M_180_PI;
	float thetaz = ATAN(n.y,n.x) * M_180_PI;
	transform = new Transform(Transform::Translate(c1.x,c1.y,c1.z) * Transform::RotateZ(thetaz) * Transform::RotateY(thetay) * Transform::Scale(r,r,h));
}

void Cylinder::GetBounds(Vector &min, Vector &max)
{
	min = Vector(-1,-1,0);
	max = Vector(1,1,1);
	if(transform) transform->TransformBounds(min,max);
}

void CylinderUV(Vector P, float& u, float& v)
{
	u = 0.5f + ATAN(P.y,P.x) * M_1_2PI;
	v = P.z;
}

// Solve Ax^2 + Bx + C = 0, return number of real roots
int SolveQuadratic(float A, float B, float C, float* t1, float* t2)
{
	if(A == 0.f)
	{
		if(B == 0) return 0;
		*t1 = -C / B;
		return 1;
	}
	else
	{
		float delta = B*B - 4*A*C;
		if(delta < 0.f) return 0;
		A = 2*A;
		*t1 = -B / A;
		if(delta == 0.f)
		{
			return 1;
		}
		else
		{
			delta = SQRT(delta) / A;
			*t2 = *t1 - delta;
			*t1 = *t1 + delta;
			return 2;
		}
	}
}

bool Cylinder::Intersect(Ray& r, Intersection& i)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	float C = ray.O.x*ray.O.x + ray.O.y*ray.O.y - 1;
	if(fabs(C) < EPSILON) return false;

	float A = ray.D.x*ray.D.x + ray.D.y*ray.D.y;
	float B = 2 * (ray.D.x*ray.O.x + ray.D.y*ray.O.y);
	float t1, t2;

	int c = SolveQuadratic(A,B,C,&t1,&t2);

	if(c == 0)
	{
		return false;
	}
	else if(c == 1)
	{
		if(r.inRange(t1))
		{
			Vector P(0,0,ray.O.z + t1 * ray.D.z);
			if(P.z >= 0.f && P.z <= 1.f)
			{
				P.x = ray.O.x + t1 * ray.D.x;
				P.y = ray.O.y + t1 * ray.D.y;
				float u, v;
				CylinderUV(P,u,v);
				i.set(this,P,Vector(P.x,P.y,0).normal(),u,v);
				r.trunc(t1);
				return true;
			}
		}
	}
	else
	{
		if(t1 > t2) SWAP(t1,t2);

		if(r.inRange(t1))
		{
			Vector P(0, 0, ray.O.z + t1 * ray.D.z);
			if(P.z >= 0.f && P.z <= 1.f)
			{
				P.x = ray.O.x + t1 * ray.D.x;
				P.y = ray.O.y + t1 * ray.D.y;
				float u, v;
				CylinderUV(P,u,v);
				i.set(this,P,Vector(P.x,P.y,0).normal(),u,v);
				r.trunc(t1);
				return true;
			}
		}
		if(r.inRange(t2))
		{
			Vector P(0, 0, ray.O.z + t2 * ray.D.z);
			if(P.z >= 0.f && P.z <= 1.f)
			{
				P.x = ray.O.x + t2 * ray.D.x;
				P.y = ray.O.y + t2 * ray.D.y;
				float u, v;
				CylinderUV(P,u,v);
				i.set(this,P,Vector(P.x,P.y,0).normal(),u,v);
				r.trunc(t2);
				return true;
			}
		}
	}
	return false;
}

bool Cylinder::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	float C = ray.O.x*ray.O.x + ray.O.y*ray.O.y - 1;
	if(fabs(C) < EPSILON) return false;

	float A = ray.D.x*ray.D.x + ray.D.y*ray.D.y;
	float B = 2 * (ray.D.x*ray.O.x + ray.D.y*ray.O.y);
	float t1, t2;

	int c = SolveQuadratic(A,B,C,&t1,&t2);

	if(c == 0)
	{
		return false;
	}
	else if(c == 1)
	{
		if(r.inRange(t1))
		{
			float z = ray.O.z + t1 * ray.D.z;
			if(z >= 0.f && z <= 1.f)
				return true;
		}
	}
	else
	{
		if(t1 > t2) SWAP(t1,t2);

		if(r.inRange(t1))
		{
			float z = ray.O.z + t1 * ray.D.z;
			if(z >= 0.f && z <= 1.f)
				return true;
		}
		if(r.inRange(t2))
		{
			float z = ray.O.z + t2 * ray.D.z;
			if(z >= 0.f && z <= 1.f)
				return true;
		}
	}
	return false;
}
