#pragma once
#include <string.h>
#include "Defines.h"

class Matrix
{
public:
	float m[4][4];

	Matrix(void)
	{
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				m[i][j] = (i == j ? 1.0f : 0.0f);
	}

	Matrix(const Matrix& a)
	{
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				m[i][j] = a.m[i][j];
	}

	Matrix(float a[4][4])
	{
		memcpy(m, a, 16*sizeof(float));
	}

	Matrix(float a00, float a01, float a02, float a03, 
				   float a10, float a11, float a12, float a13, 
				   float a20, float a21, float a22, float a23, 
				   float a30, float a31, float a32, float a33 )
	{
		m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
		m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
		m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
		m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
	}

	Matrix operator *(const Matrix& n) const
	{
		float a[4][4];
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				a[i][j] = 0;
				for(int k = 0; k < 4; k++) {
					a[i][j] += m[i][k] * n.m[k][j];
				}
			}
		}
		return Matrix(a);
	}

	Matrix operator +(const Matrix& n) const
	{
		float a[4][4];
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				a[i][j] = m[i][j] + n.m[i][j];

		return Matrix(a);
	}

	Matrix operator -(const Matrix& n) const
	{
		float a[4][4];
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				a[i][j] = m[i][j] - n.m[i][j];

		return Matrix(a);
	}

	Matrix operator *(const float s) const
	{
		float a[4][4];
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				a[i][j] = m[i][j] * s;

		return Matrix(a);
	}


	Matrix Transpose() const
	{
		return Matrix(m[0][0], m[1][0], m[2][0], m[3][0],
					  m[0][1], m[1][1], m[2][1], m[3][1],
					  m[0][2], m[1][2], m[2][2], m[3][2],
					  m[0][3], m[1][3], m[2][3], m[3][3]);
	}

	Matrix Inverse() const;

	void print()
	{
		for(int i = 0; i < 4; i++)
		{
			printf("\n|");
			for(int j = 0; j < 4; j++)
				printf("%5.5f ", m[j][i]); 
			printf("|");
		}
	}
};
