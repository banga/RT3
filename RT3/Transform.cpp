#include "Transform.h"

// Transform creation functions:
Transform Transform::Translate(float dx, float dy, float dz)
{
	Matrix m (1, 0, 0, dx,
			  0, 1, 0, dy,
			  0, 0, 1, dz,
			  0, 0, 0, 1);
	Matrix minv (1, 0, 0, -dx,
				 0, 1, 0, -dy,
				 0, 0, 1, -dz,
				 0, 0, 0, 1);
	return Transform(m, minv);
}

Transform Transform::RotateX(float theta)
{
	theta = DEG_2_RAD(theta);
	float sint = sinf(theta);
	float cost = cosf(theta);

	Matrix m (1,    0,    0, 0,
			  0, cost, -sint, 0,
			  0, sint,  cost, 0,
			  0,    0,     0, 1);
	return Transform(m, m.Transpose());
}

Transform Transform::RotateY(float theta)
{
	theta = DEG_2_RAD(theta);
	float sint = sinf(theta);
	float cost = cosf(theta);

	Matrix m (  cost, 0, sint, 0,
			       0, 1,    0, 0,
			   -sint, 0, cost, 0,
			       0, 0,    0, 1);
	return Transform(m, m.Transpose());
}

Transform Transform::RotateZ(float theta)
{
	theta = DEG_2_RAD(theta);
	float sint = sinf(theta);
	float cost = cosf(theta);

	Matrix m ( cost, -sint, 0, 0,
			   sint,  cost, 0, 0,
			      0,     0, 1, 0,
			      0,     0, 0, 1);
	return Transform(m, m.Transpose());
}

Transform Transform::Scale(float sx, float sy, float sz)
{
	Matrix m ( sx, 0, 0, 0,
			   0, sy, 0, 0,
			   0, 0, sz, 0,
			   0, 0, 0, 1);
	Matrix minv ( 1.0f / sx, 0, 0, 0,
				  0, 1.0f / sy, 0, 0,
			      0, 0, 1.0f / sz, 0,
			      0, 0, 0, 1);
	return Transform(m, minv);
}