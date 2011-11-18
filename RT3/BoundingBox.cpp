#include "BoundingBox.h"

BoundingBox::BoundingBox()
{
}

BoundingBox::BoundingBox(Vector &_a, Vector &_b):min(_a), max(_b)
{
}

BoundingBox::BoundingBox(const BoundingBox& bb):min(bb.min), max(bb.max)
{
}

bool BoundingBox::Intersect(const Ray& r)
{
	float t0 = r.maxt, t1 = r.mint;

	for(int i = 0; i < 3; i++) {
		float invD = 1.0f / r.D[i];
		float tn = (min[i] - r.O[i]) * invD;
		float tf = (max[i] - r.O[i]) * invD;

		if(tn > tf) {
			float t = tn;
			tn = tf;
			tf = t;
		}
		t0 = (tn > t0) ? tn : t0;
		t1 = (tf < t1) ? tf : t1;
		if(t0 > t1) return false;
	}
	return true;
}

void BoundingBox::Union(const Vector &p)
{
	min.x = ((p.x < min.x) ? p.x : min.x);
	min.y = ((p.y < min.y) ? p.y : min.y);
	min.z = ((p.z < min.z) ? p.z : min.z);

	max.x = ((p.x > max.x) ? p.x : max.x);
	max.y = ((p.y > max.y) ? p.y : max.y);
	max.z = ((p.z > max.z) ? p.z : max.z);
}

BoundingBox BoundingBox::operator = (BoundingBox& bb)
{
	min = bb.min;
	max = bb.max;
	return *this;
}