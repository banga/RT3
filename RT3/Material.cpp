
#include "Material.h"

Material::Material(Texture<Spectrum>* dc)
{
	Kd = dc;
	Ka = transmit = mirror = NULL;
}

Material::Material(Texture<Spectrum>* dc, Texture<Spectrum>* ac)
{
	Kd = dc;
	Ka = ac;
	transmit = mirror = NULL;
}

Material::Material(Texture<Spectrum>* dc, Texture<Spectrum>* ac, Texture<Spectrum>* mc)
{
	Kd = dc;
	Ka = ac;
	mirror = mc;
	transmit = NULL;
}

Material::Material(Texture<Spectrum>* dc, Texture<Spectrum>* ac, Texture<Spectrum>* mc, Texture<Spectrum>* tc, float i)
{
	Kd = dc;
	Ka = ac;
	mirror = mc;
	transmit = tc;
	ior = i;
}

Material::Material(const Material& m):
Kd(m.Kd), Ka(m.Ka), mirror(m.mirror), transmit(m.transmit), ior(m.ior)
{
}

void Material::generateSample(Spectrum& f, Vector& sampleRayDir, const Vector& incident, const Intersection& i) const
{
	Vector p1, p2;
	generatePerpendiculars(i.normal, p1, p2);
	float r1 = 2 * M_PI * FRAND(), r2 = FRAND(), r2s = sqrt(r2);
	sampleRayDir = ((p1 * (cosf(r1) * r2s) + p2 * (sinf(r1) * r2s) + i.normal * (1.0f - r2))).normal();
	f = (Kd != NULL) ? Kd->eval(i) : Spectrum(0.);
}

Material::~Material()
{
	if(Kd) delete Kd;
	if(Ka) delete Ka;
	if(mirror) delete mirror;
	if(transmit) delete transmit;
}

void generatePerpendiculars(const Vector& normal, Vector& p1, Vector& p2)
{
	if(fabs(normal.x) > EPSILON) {
		p1.x = -normal.z;
		p1.z = normal.x;
		p1 = p1.normal();
		p2 = Vector::crossProduct(normal, p1);
	} else if(fabs(normal.y) > EPSILON) {
		p1.y = normal.z;
		p1.z = -normal.y;
		p1 = p1.normal();
		p2 = Vector::crossProduct(normal, p1);
	} else {
		p1.x = 1;
		p2.y = 1;
	}
}