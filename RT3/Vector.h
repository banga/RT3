#pragma once
#include "Defines.h"

class Vector
{
public:
	float x, y, z;
	enum Axis {X = 0, Y = 1, Z = 2};

	Vector::Vector(void)
	{
		x = y = z = 0;
	}

	Vector::Vector(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	Vector::Vector(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	float Vector::magnitude() const
	{
		return SQRT(x*x+y*y+z*z);
	}

	Vector Vector::normal() const
	{
		float m = SQRT(x*x+y*y+z*z);
		if(m > (float)0) 
			return Vector(x, y, z) / m;
		else
			return *this;
	}

	Vector Vector::operator =(Vector v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	Vector Vector::operator *(float s) const
	{
		return Vector(x*s, y*s, z*s);
	}

	Vector Vector::operator +(Vector v) const
	{
		return Vector(x+v.x, y+v.y, z+v.z);
	}

	Vector Vector::operator -(Vector v) const
	{
		return Vector(x-v.x, y-v.y, z-v.z);
	}

	float& Vector::operator [](int i)
	{
		return ((i == 0) ? x : ((i == 1) ? y : z));
	}

	float Vector::operator [](int i) const
	{
		return ((i == 0) ? x : ((i == 1) ? y : z));
	}

	Vector Vector::operator -() const
	{
		return Vector(-x,-y,-z);
	}

	Vector Vector::operator /(float f) const
	{
		f = 1.0f / f;
		return Vector(x*f, y*f, z*f);
	}

	static Vector crossProduct(const Vector &v1, const Vector &v2)
	{
		return Vector(v1.y * v2.z - v1.z * v2.y,
					  v1.z * v2.x - v1.x * v2.z,
					  v1.x * v2.y - v1.y * v2.x);
	}

	static float Vector::dotProduct(const Vector &v1, const Vector &v2)
	{
		return ((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z));
	}

	static float Vector::absDotProduct(const Vector &v1, const Vector &v2)
	{
		return fabs((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z));
	}

	static Vector Vector::multiplyComponents(const Vector &v1, const Vector &v2)
	{
		return Vector(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
	}

	void print() const
	{
		printf("(%.2f,%.2f,%.2f)",x,y,z);
	}
};