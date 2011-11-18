#pragma once
#include "Defines.h"
#include "Object3d.h"
#include "Camera.h"
#include "Light.h"
#include "Image.h"
#include <Windows.h>

class Scene
{
private:
	float scale, gamma, invWidth, invHeight;
	static const float rWeight[3];
	static const float gWeight[3];
	static const float bWeight[3];
	bool aborted;

public:
	Object3d** objects; int nObjects;
	Light** lights;	int nLights;
	vector<Material*> materials;
	Camera* camera;
	int maxDepth;
	Spectrum Background;
	int samplesPerPixel;
	int Width, Height, xTiles, yTiles, tileWidth, tileHeight;
	Image* image;
	int pixelsTraced;
	int threadCount;
	bool rendering;

	void (*onRenderComplete)();

	Scene(int samples, int width, int height, int xTiles, int yTiles, void (*onRenderComplete)(),
		Camera *camera, vector<Object3d*>& objects, vector<Light*>& lights, vector<Material*>& materials);
	Spectrum Trace(Ray ray, int depth);
	void RenderRow(int row);
	void RenderTile(int,int);
	void Render();
	void abortRender();
	void setImageParams(int width, int height, int xTiles, int yTiles);
	void setSamples(int samples);
	/* Testing */
	int total_internal_reflections;
public:
	~Scene(void);
};