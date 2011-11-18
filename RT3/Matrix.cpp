
#include "Matrix.h"

Matrix Matrix::Inverse() const
{
	float n[4][4], minv[4][4];

	memcpy(n, m, 16*sizeof(float));

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			minv[i][j] = (i==j ? 1.0f : 0.0f);

	for(int i = 0; i < 4; i++) {
		int pcol = i;
		float p = n[i][pcol];
		// Swap columns for zero pivots:
		if(p == 0.0f) {
			int i2 = -1;
			for(int k = i+1; k < 4; k++) {
				if(n[k][pcol] != 0.0f) {
					i2 = k;
				}
			}
			if(i2 == -1) {
				printf("Singular Matrix in Matrix::Inverse()");
			}
			for(int j = 0; j < 4; j++) {
				SWAP(n[i][j], n[i2][j]);
				SWAP(minv[i][j], minv[i2][j]);
			}
		}
		// Make pivot value 1
		if(p != 1.0f) {
			float s = 1.0f / p;
			for(int j = 0; j < 4; j++) {
				n[i][j] *= s;
				minv[i][j] *= s;
			}
		}
		// Make all other columns zero:
		for(int k = 0; k < 4; k++) {
			if((k != i) && (n[k][pcol] != 0)) {
				float s = n[k][pcol];
				for(int j = 0; j < 4; j++) {
					n[k][j] -= n[i][j] * s;
					minv[k][j] -= minv[i][j] * s;
				}
			}
		}
	}
	return Matrix(minv);
}
