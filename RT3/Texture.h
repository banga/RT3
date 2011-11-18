#pragma once
#include <string>
#include "Transform.h"

using std::string;

template <class T>
class Texture
{
	public:
		Transform *transform;
		Texture(Transform* t)
		{
			transform = (t == NULL) ? NULL : new Transform(*t);
		}
		virtual T eval(const Intersection& intersection) const
		{
			throw "Eval of base class Texture called";
		}
		virtual string toString()
		{
			return "Texture";
		}
		~Texture()
		{
			if(transform) delete transform;
		}
};