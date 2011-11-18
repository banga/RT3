#pragma once
#include "Texture.h"

template <class T>
class ConstantTexture : public Texture<T>
{
	public:
		T val;
		ConstantTexture(T t):val(t),Texture(NULL) {}
		T eval(const Intersection& intersection) const
		{
			return val;
		}
};