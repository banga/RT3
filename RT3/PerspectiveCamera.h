#pragma once
#include "Defines.h"
#include "Camera.h"

class PerspectiveCamera:public Camera
{
public:
	Vector center, imageCorner;
	float distance;

	PerspectiveCamera(Vector origin, Vector up, Vector right, float angle);
	Ray getRayAt(float x, float y) ;
	void lookAt(Vector point) ;

public:
	~PerspectiveCamera(void);
};
