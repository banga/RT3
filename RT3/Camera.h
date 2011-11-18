#pragma once
#include "Defines.h"
#include "Ray.h"
	
class Camera
{
public:
	Vector up, right, forward, origin;

	Camera(Vector origin, Vector up, Vector right);

	///<summary>Look at given point, and adjust right vector accordingly</summary>
	virtual void lookAt(Vector Point);

	///<summary>Get ray at a given pixel in the image plane</summary>
	///<param name='x'>x co-ordinate between 0 and width-1</param>
	///<param name='y'>y co-ordinate between 0 and height-1</param>
	virtual Ray getRayAt(float x, float y);

public:
	~Camera(void);
};
