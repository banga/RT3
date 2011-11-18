#pragma once
#include "Defines.h"
#include "Spectrum.h"
#include "Vector.h"
#include "Texture.h"
#include <string>
using std::string;

class Material
{
public:
	Texture<Spectrum> *Ka, *Kd, *mirror, *transmit;
	float ior;

	Material(Texture<Spectrum>* diffuse);
	Material(Texture<Spectrum>* diffuse, Texture<Spectrum>* ambient);
	Material(Texture<Spectrum>* diffuse, Texture<Spectrum>* ambient, Texture<Spectrum>* mirror);
	Material(Texture<Spectrum>* diffuse, Texture<Spectrum>* ambient, Texture<Spectrum>* mirror, Texture<Spectrum>* transmit, float ior);
	Material(const Material& m);
	~Material();

	virtual void generateSample(Spectrum& f, Vector &sampleRayDir, const Vector &incident, const Intersection& i) const;
	virtual string toString() const
	{
		string str = "Material: ";
		if(Kd) str = str + "Kd: [" + Kd->toString() + "] ";
		if(Ka) str = str + "Ka: [" + Ka->toString() + "] ";
		if(mirror) str = str + "mirror: [" + mirror->toString() + "] ";
		if(transmit) str = str + "transmit: [" + transmit->toString() + "] ";
		return str;
	}
};

inline void generatePerpendiculars(const Vector& n, Vector& p1, Vector& p2);