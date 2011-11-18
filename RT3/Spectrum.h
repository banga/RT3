#pragma once
#include "Defines.h"

#define COLOR_SAMPLES 3

const int CIEstart = 360;
const int CIEend = 830;
const int nCIE = CIEend-CIEstart+1;

/*
	Spectrum class based on PBRT's class of the same name
*/
class Spectrum
{
	static float XWeight[COLOR_SAMPLES];
	static float YWeight[COLOR_SAMPLES];
	static float ZWeight[COLOR_SAMPLES];
	float c[COLOR_SAMPLES];
public:
	Spectrum();
	Spectrum(float v);
	Spectrum(float cs[COLOR_SAMPLES]);
	Spectrum(const Spectrum& spectrum);

	Spectrum operator +=(const Spectrum& s2);
	Spectrum operator + (const Spectrum& s2);
	Spectrum operator -=(const Spectrum& s2);
	Spectrum operator - (const Spectrum& s2);
	Spectrum operator *=(const Spectrum& s2);
	Spectrum operator * (const Spectrum& s2);
	Spectrum operator /=(const Spectrum& s2);
	Spectrum operator / (const Spectrum& s2);
//	Spectrum operator -();
//	friend Spectrum Exp(const Spectrum& s);

	void addWeighted(float w, const Spectrum& s);

	bool operator==(const Spectrum& s) const;
	bool isBlack() const;
	bool isNan() const;

	Spectrum sqrt() const;
	Spectrum pow(const Spectrum&e) const;

	Spectrum clamp(float low, float high) const;

	static Spectrum fromXYZ(float x, float y, float z);

	void XYZ(float xyz[3]) const;
	float y() const;
	bool operator<(const Spectrum& s2) const;

	void print() {
		printf("\n[%f %f %f]", c[0], c[1], c[2]);
	}
};
