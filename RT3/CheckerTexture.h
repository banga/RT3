#pragma once
#include "Texture.h"

template <class T>
class CheckerTexture : public Texture<T>
{
	public:
		T val1, val2;
		CheckerTexture(T t1, T t2, Transform* t):val1(t1),val2(t2),Texture(t) {}
		T eval(const Intersection& intersection) const
		{
			Vector p(intersection.u,intersection.v,0);
			if(transform) p = transform->TransformPoint(p);
			int i = MOD((int)(p.x * 2),2);
			int j = MOD((int)(p.y * 2),2);
			if((i ^ j) == 1) return val1;
			return val2;
		}
};