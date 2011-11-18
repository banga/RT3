#pragma once
#include "camera.h"

class OrthographicCamera:public Camera
{
public:
	Vector imageCorner;

	OrthographicCamera(Vector up, Vector right, Vector origin);
	Ray getRayAt(float x, float y) ;
public:
	~OrthographicCamera(void);
};
