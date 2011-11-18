
#include "Triangle.h"

Triangle::Triangle(Vector _a, Vector b, Vector c, Material* m):Object3d(m), a(_a), ab(b-a), ac(c-a)
{
	normal = Vector::crossProduct(ac,ab).normal();
}

void Triangle::GetBounds(Vector &min, Vector &max)
{
	Vector b = a + ab, c = a + ac;

#define MIN3(a,b,c) ((a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c))
	min.x = MIN3(a.x, b.x, c.x);
	min.y = MIN3(a.y, b.y, c.y);
	min.z = MIN3(a.z, b.z, c.z);
#undef MIN3

#define MAX3(a,b,c) ((a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c))
	max.x = MAX3(a.x, b.x, c.x);
	max.y = MAX3(a.y, b.y, c.y);
	max.z = MAX3(a.z, b.z, c.z);
#undef MAX3

	if(transform) transform->TransformBounds(min, max);
}

bool Triangle::Intersect(Ray& r, Intersection& i)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	Vector s1 = Vector::crossProduct(ray.D, ac);
	float divisor = Vector::dotProduct(s1, ab);
	if(divisor == 0.)
		return false;
	float invDivisor = 1.0f / divisor;

	Vector d = ray.O - a;
	float b1 = Vector::dotProduct(d, s1) * invDivisor;
	if(b1 < 0.0 || b1 > 1.0f)
		return false;

	Vector s2 = Vector::crossProduct(d, ab);
	float b2 = Vector::dotProduct(ray.D, s2) * invDivisor;
	if(b2 < 0.0 || b1 + b2 > 1.0f)
		return false;

	float t = Vector::dotProduct(ac, s2) * invDivisor;
	if(r.trunc(t)) {
		i.set(this,ray.getPointAt(t),normal,b1,b2);
		return true;
	}
	return false;
}

bool Triangle::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	Vector s1 = Vector::crossProduct(ray.D, ac);
	float divisor = Vector::dotProduct(s1, ab);
	if(fabs(divisor) < EPSILON)
		return false;
	float invDivisor = 1.0f / divisor;

	Vector d = ray.O - a;
	float b1 = Vector::dotProduct(d, s1) * invDivisor;
	if(b1 < 0.0 || b1 > 1.0f)
		return false;

	Vector s2 = Vector::crossProduct(d, ab);
	float b2 = Vector::dotProduct(ray.D, s2) * invDivisor;
	if(b2 < 0.0 || b1 + b2 > 1.0f)
		return false;

	float t = Vector::dotProduct(ac, s2) * invDivisor;
	return r.trunc(t);
}

Triangle::~Triangle(void)
{
}
