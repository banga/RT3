#pragma once
#include "Scene.h"
#include <map>

using std::map;

extern int yyerror(char*);

#define _VAL(type,x) (*(type*)x)
#define _VEC(x) _VAL(Vector,x)
#define _SPC(x) _VAL(Spectrum,x)

class Variable {
	public:
	enum Type
	{
		VAR_INTEGER  = 0,
		VAR_FLOAT    = 1,
		VAR_VECTOR   = 2,
		VAR_SPECTRUM = 3,
		VAR_MATERIAL = 4,
		VAR_OBJECT   = 5,
		VAR_LIGHT    = 6,
		VAR_CAMERA   = 7,
		VAR_NONE     = 8
	};

	union
	{
		int integer;
		float real;
		void* pData;
	};
	Type type;
	bool unused;

	Variable()
	{
		type = VAR_NONE;
		pData = NULL;
	}
	Variable(int i)
	{
		type = VAR_INTEGER;
		integer = i;
		unused = true;
	}
	Variable(float f)
	{
		type = VAR_FLOAT;
		real = f;
		unused = true;
	}
	Variable(Vector* v)
	{
		type = VAR_VECTOR;
		pData = new Vector(*v);
		unused = true;
	}
	Variable(Spectrum* s)
	{
		type = VAR_SPECTRUM;
		pData = new Spectrum(*s);
		unused = true;
	}
	Variable(Material* m)
	{
		type = VAR_MATERIAL;
		pData = new Material(*m);
		unused = true;
	}
	Variable(Object3d* o)
	{
		type = VAR_OBJECT;
		pData = new Object3d(*o);
		unused = true;
	}
	Variable(Camera* c)
	{
		type = VAR_CAMERA;
		pData = new Camera(*c);
		unused = true;
	}
	Variable(Light* l)
	{
		type = VAR_LIGHT;
		pData = new Light(*l);
		unused = true;
	}
	bool getInteger(int* i)
	{
		if(type == VAR_INTEGER)
		{
			*i = integer;
			unused = false;
			return true;
		}
		return false;
	}
	bool getFloat(float* f)
	{
		if(type == VAR_FLOAT)
		{
			*f = real;
			unused = false;
			return true;
		}
		return false;
	}
	bool getVector(Vector** v)
	{
		if(type == VAR_VECTOR)
		{
			*v = (Vector*)pData;
			unused = false;
			return true;
		}
		return false;
	}
	bool getSpectrum(Spectrum** s)
	{
		if(type == VAR_SPECTRUM)
		{
			*s = (Spectrum*)pData;
			unused = false;
			return true;
		}
		return false;
	}
	bool getMaterial(Material** m)
	{
		if(type == VAR_MATERIAL)
		{
			*m = (Material*)pData;
			unused = false;
			return true;
		}
		return false;
	}
	bool getObject(Object3d** o)
	{
		if(type == VAR_OBJECT)
		{
			*o = (Object3d*)pData;
			unused = false;
			return true;
		}
		return false;
	}
	bool getCamera(Camera** c)
	{
		if(type == VAR_CAMERA)
		{
			*c = (Camera*)pData;
			unused = false;
			return true;
		}
		return false;
	}
	bool getLight(Light** l)
	{
		if(type == VAR_LIGHT)
		{
			*l = (Light*)pData;
			unused = false;
			return true;
		}
		return false;
	}
	bool get(void* ptr, Type t)
	{
		switch(t)
		{
			case VAR_INTEGER:  return getInteger ((int*)ptr);
			case VAR_FLOAT:    return getFloat   ((float*)ptr);
			case VAR_VECTOR:   return getVector  ((Vector**)&ptr);
			case VAR_SPECTRUM: return getSpectrum((Spectrum**)&ptr);
			case VAR_MATERIAL: return getMaterial((Material**)&ptr);
			case VAR_OBJECT:   return getObject  ((Object3d**)&ptr);
			case VAR_CAMERA:   return getCamera  ((Camera**)&ptr);
			case VAR_LIGHT:    return getLight   ((Light**)&ptr);
		}
		return false;
	}
	~Variable()
	{
		if(pData != NULL)
		{
			switch(type)
			{
				case VAR_MATERIAL:
				case VAR_OBJECT:
				case VAR_CAMERA:
				case VAR_LIGHT:
					if(unused) delete pData; break;
			}
		}
	}
};