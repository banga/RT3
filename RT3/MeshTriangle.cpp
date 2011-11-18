#include "MeshTriangle.h"

MeshTriangle::MeshTriangle(Vector _a, Vector b, Vector c, Vector _n1, Vector _n2, Vector _n3, Material* m):
Object3d(m), a(_a), ab(b-_a), ac(c-_a), n1(_n1), n12(_n2-_n1), n13(_n3-_n1)
{
	u1 = v1 = 0;
	u12 = 1; v12 = 0;
	u13 = 0; v13 = 1;
}

MeshTriangle::MeshTriangle(Vector _a, Vector b, Vector c, float u[3], float v[3], Material* m):
Object3d(m), a(_a), ab(b-_a), ac(c-_a)
{
	u1  = u[0]; v1 = v[0];
	u12 = u[1]-u1; v12 = v[1]-v1;
	u12 = u[2]-u1; v12 = v[2]-v1;
	n1 = Vector::crossProduct(ab,ac).normal();
	n12 = n1;
	n13 = n1;
}

MeshTriangle::MeshTriangle(Vector _a, Vector b, Vector c, Vector _n1, Vector _n2, Vector _n3, float u[3], float v[3], Material* m):
Object3d(m), a(_a), ab(b-_a), ac(c-_a), n1(_n1), n12(_n2-_n1), n13(_n3-_n1)
{
	u1  = u[0]; v1 = v[0];
	u12 = u[1]-u1; v12 = v[1]-v1;
	u12 = u[2]-u1; v12 = v[2]-v1;
}

void MeshTriangle::GetBounds(Vector &min, Vector &max)
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

	if(transform) transform->TransformBounds(min,max);
}

bool MeshTriangle::Intersect(Ray& r, Intersection& i)
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
		float u = u1 + u12 * b1 + u13 * b2;
		float v = v1 + v12 * b1 + v13 * b2;
		i.set(this,ray.getPointAt(t),(n1 + n12*b1 + n13*b2).normal(),u,v);
		return true;
	}
	return false;
}

bool MeshTriangle::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	Vector s1 = Vector::crossProduct(ray.D, ac);
	float divisor = Vector::dotProduct(s1, ab);
	if(divisor == 0.f)
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

MeshTriangle::~MeshTriangle(void)
{
}
