#pragma once
#include "Matrix.h"
#include "BoundingBox.h"
#include "Ray.h"

class Transform
{
public:
	Matrix m, minv;

	Transform()
	{
	}

	Transform(Matrix &a):
		m(a),
		minv(a.Inverse())
	{
	}

	Transform(Matrix &a, Matrix &ainv):
		m(a),
		minv(ainv)
	{
	}

	Transform(const Transform& t):
		m(t.m),
		minv(t.minv)
	{
	}

	inline Vector TransformPoint(const Vector &p) const
	{
		float x = p.x, y = p.y, z = p.z;
		float xt = m.m[0][0]*x + m.m[0][1]*y + m.m[0][2]*z + m.m[0][3];
		float yt = m.m[1][0]*x + m.m[1][1]*y + m.m[1][2]*z + m.m[1][3];
		float zt = m.m[2][0]*x + m.m[2][1]*y + m.m[2][2]*z + m.m[2][3];
		float wt = m.m[3][0]*x + m.m[3][1]*y + m.m[3][2]*z + m.m[3][3];

		if(wt == 0.0)
			throw "w is zero in TransformPoint";

		return Vector(xt, yt, zt)/wt;
	}

	inline Vector InvTransformPoint(const Vector &p) const
	{
		float x = p.x, y = p.y, z = p.z;
		float xt = minv.m[0][0]*x + minv.m[0][1]*y + minv.m[0][2]*z + minv.m[0][3];
		float yt = minv.m[1][0]*x + minv.m[1][1]*y + minv.m[1][2]*z + minv.m[1][3];
		float zt = minv.m[2][0]*x + minv.m[2][1]*y + minv.m[2][2]*z + minv.m[2][3];
		float wt = minv.m[3][0]*x + minv.m[3][1]*y + minv.m[3][2]*z + minv.m[3][3];

		if(wt == 0.0)
			throw "w is zero in TransformPoint";

		return Vector(xt, yt, zt)/wt;
	}

	inline Vector TransformDirection(const Vector &d) const
	{
		float x = d.x, y = d.y, z = d.z;
		return Vector( m.m[0][0]*x + m.m[0][1]*y + m.m[0][2]*z,
					   m.m[1][0]*x + m.m[1][1]*y + m.m[1][2]*z,
					   m.m[2][0]*x + m.m[2][1]*y + m.m[2][2]*z);
	}

	inline Vector InvTransformDirection(const Vector &d) const
	{
		float x = d.x, y = d.y, z = d.z;
		return Vector( minv.m[0][0]*x + minv.m[0][1]*y + minv.m[0][2]*z,
					   minv.m[1][0]*x + minv.m[1][1]*y + minv.m[1][2]*z,
					   minv.m[2][0]*x + minv.m[2][1]*y + minv.m[2][2]*z);
	}

	inline Vector TransformNormal(const Vector &n) const
	{
		float x = n.x, y = n.y, z = n.z;
		return Vector(minv.m[0][0]*x + minv.m[1][0]*y + minv.m[2][0]*z,
					  minv.m[0][1]*x + minv.m[1][1]*y + minv.m[2][1]*z,
					  minv.m[0][2]*x + minv.m[1][2]*y + minv.m[2][2]*z).normal();
	}

	inline Vector InvTransformNormal(const Vector &n) const 
	{
		float x = n.x, y = n.y, z = n.z;
		return Vector(m.m[0][0]*x + m.m[1][0]*y + m.m[2][0]*z,
					  m.m[0][1]*x + m.m[1][1]*y + m.m[2][1]*z,
					  m.m[0][2]*x + m.m[1][2]*y + m.m[2][2]*z).normal();
	}

	inline Intersection TransformIntersection(const Intersection& i) const
	{
		return Intersection(i.object, TransformPoint(i.point), TransformNormal(i.normal), i.u, i.v);
	}

	inline Intersection InvTransformIntersection(const Intersection& i) const
	{
		return Intersection(i.object, InvTransformPoint(i.point), InvTransformNormal(i.normal), i.u, i.v);
	}

	Ray TransformRay(const Ray& r) const
	{
		return Ray(TransformPoint(r.O), TransformDirection(r.D), r.mint, r.maxt);
	}

	Ray InvTransformRay(const Ray& r) const
	{
		return Ray(InvTransformPoint(r.O), InvTransformDirection(r.D), r.mint, r.maxt);
	}

	void TransformBounds(Vector& min, Vector& max)  const
	{
		Vector tmin = TransformPoint(min);
		BoundingBox b(tmin,tmin);
		b.Union(TransformPoint(max));
		b.Union(TransformPoint(Vector(min.x, min.y, max.z)));
		b.Union(TransformPoint(Vector(min.x, max.y, min.z)));
		b.Union(TransformPoint(Vector(min.x, max.y, max.z)));
		b.Union(TransformPoint(Vector(max.x, min.y, min.z)));
		b.Union(TransformPoint(Vector(max.x, min.y, max.z)));
		b.Union(TransformPoint(Vector(max.x, max.y, min.z)));
		min = b.min;
		max = b.max;
	}

	Transform operator *(const Transform& t) const
	{
		return Transform(m * t.m, t.minv * minv);
	}

	static Transform Translate(float dx, float dy, float dz);
	static Transform RotateX(float theta);
	static Transform RotateY(float theta);
	static Transform RotateZ(float theta);
	static Transform Scale(float sx, float sy, float sz);
};

// Transform creation functions: