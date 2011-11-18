#include "Scene.h"
#include "Defines.h"

#define _AA
//#define _MONTECARLO

const float Scene::rWeight[3] = { 3.240479f, -1.537150f, -0.498535f };
const float Scene::gWeight[3] = {-0.969256f,  1.875991f,  0.041556f };
const float Scene::bWeight[3] = { 0.055648f, -0.204043f,  1.057311f };

Scene::Scene(int s, int w, int h, int xT, int yT, void (*_onRenderComplete)(),
	Camera *c, vector<Object3d*>& _objects, vector<Light*>& _lights, vector<Material*>& _materials)
	:materials(_materials)
{
	camera = c;
	maxDepth = 3;
	total_internal_reflections = 0;
	onRenderComplete = _onRenderComplete;
	gamma = 1 / 2.2f;

	image = new Image(w,h);
	setImageParams(w,h,xT,yT);
	setSamples(s);

	objects = new Object3d*[_objects.size()];
	int i = 0;
	for(vector<Object3d*>::iterator it = _objects.begin(); it < _objects.end(); it++) {
		objects[i++] = *it;
	}
	nObjects = i;
	i = 0;
	lights = new Light*[_lights.size()];
	for(vector<Light*>::iterator it = _lights.begin(); it < _lights.end(); it++) {
		lights[i++] = *it;
	}
	nLights = i;
}

void Scene::setSamples(int s)
{
	samplesPerPixel = s;
	scale = 1 / (float)s;
}

void Scene::setImageParams(int w, int h, int xT, int yT)
{
	image->resize(w,h);
	Width = w;
	Height = h;
	invWidth = 1 / (float)w;
	invHeight = 1 / (float)h;
	xTiles = xT;
	yTiles = yT;
	tileWidth = w / xTiles;
	tileHeight = h / yTiles;
	if((tileWidth * xTiles) < Width) xTiles++;
	if((tileHeight * yTiles) < Height) yTiles++;
}

void Scene::RenderRow(int row)
{
	srand(row*row*row);
	float xyz[3], r, g, b;
	unsigned char rgb[3];

	for(int i = 0; i < Width; i++)
	{
		if(aborted) return;

		Spectrum color;
#if defined _AA || defined _MONTECARLO 
		for(int s = 0; s < samplesPerPixel; s++) {
			float r1=2*FRAND(), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
			float r2=2*FRAND(), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
			Ray ray = camera->getRayAt((dx + i) * invWidth, (dy + row) * invHeight);

			color += Trace(ray, 0);
		}
#else
		Ray ray = camera->getRayAt(i * invWidth, row * invHeight);
		color = Trace(ray, 0);
#endif
		color.XYZ(xyz);
		xyz[0] = POW(xyz[0] * scale,gamma);
		xyz[1] = POW(xyz[1] * scale,gamma);
		xyz[2] = POW(xyz[2] * scale,gamma);

		r = rWeight[0]*xyz[0] + rWeight[1]*xyz[1] + rWeight[2]*xyz[2];
		g = gWeight[0]*xyz[0] + gWeight[1]*xyz[1] + gWeight[2]*xyz[2];
		b = bWeight[0]*xyz[0] + bWeight[1]*xyz[1] + bWeight[2]*xyz[2];

		r = CLAMP(r*255, 0, 255);
		g = CLAMP(g*255, 0, 255);
		b = CLAMP(b*255, 0, 255);

		rgb[0] = (unsigned char)r;
		rgb[1] = (unsigned char)g;
		rgb[2] = (unsigned char)b;
		image->setPixel(i,Height-1-row,rgb);
		pixelsTraced++;
	}
}

void Scene::RenderTile(int i, int j)
{
	int xmin = tileWidth*i, xmax = xmin+tileWidth;
	xmax = (xmax > Width ? Width: xmax);
	int ymin = tileHeight*j, ymax = ymin+tileHeight;
	ymax = (ymax > Height? Height: ymax);

	float xyz[3], r, g, b;
	unsigned char rgb[3];

	for(int y = ymin; y < ymax; y++)
	for(int x = xmin; x < xmax; x++)
	{
		if(aborted) return;

		Spectrum color;
		for(int s = 0; s < samplesPerPixel; s++) {
#ifdef _AA
			float r1=2*FRAND(), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
			float r2=2*FRAND(), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
			Ray ray = camera->getRayAt(1 - (dx + x) * invWidth, 1 - (dy + y) * invHeight);
#else
			Ray ray = camera->getRayAt(1 - x * invWidth, 1 - y * invHeight);
#endif
			color += Trace(ray, 0);
		}

		color.XYZ(xyz);

		r = rWeight[0]*xyz[0] + rWeight[1]*xyz[1] + rWeight[2]*xyz[2];
		g = gWeight[0]*xyz[0] + gWeight[1]*xyz[1] + gWeight[2]*xyz[2];
		b = bWeight[0]*xyz[0] + bWeight[1]*xyz[1] + bWeight[2]*xyz[2];

		r = POW(r * scale,gamma);
		g = POW(g * scale,gamma);
		b = POW(b * scale,gamma);

		r = CLAMP(r*255+.5f, 0, 255);
		g = CLAMP(g*255+.5f, 0, 255);
		b = CLAMP(b*255+.5f, 0, 255);

		rgb[0] = (unsigned char)r;
		rgb[1] = (unsigned char)g;
		rgb[2] = (unsigned char)b;
		image->setPixel(x,y,rgb);
		pixelsTraced++;
	}
}

struct RenderData
{
	Scene *pScene;
	int row, column;
	int id;
};

DWORD WINAPI RenderTileThreaded(LPVOID pRenderData)
{
	RenderData *rd = (RenderData*)pRenderData;
	rd->pScene->RenderTile(rd->row,rd->column);
	rd->pScene->threadCount--;
	delete rd;
	return 0;
}

void Scene::Render()
{
	if(camera == NULL)
		return;
	
	rendering = true;
	aborted = false;

	pixelsTraced = 0;

/*
	int j = 0;
	while(j < Height) {
		RenderRow(j++);
	}
// */

// /*
	for(int j = 0; j < yTiles; j++) {
		for(int i = 0; i < xTiles; i++) {
			RenderTile(i,j);
		}
	}
// */
	rendering = false;
	if(!aborted) onRenderComplete();
}

void Scene::abortRender()
{
	aborted = true;
}

inline Vector getBackground1(Vector &D) {
	// x = cosf(t)*cosf(p)
	// y = cosf(t)*sinf(p)
	// z = sinf(t)
	float p = atan(D.y / D.x);
	if(p < 0.0) p += 2 * M_PI;

	float t = acos(D.z);

	float u = p * M_1_PI * 0.5f;
	float v = t * M_1_PI * 0.5f;

	Vector c;
	const float t0 = 0.02f, t1 = 0.1f, t2 = 1-t1, t3 = 1-t0;

	float tx = fmod(u*50, 1), ty = fmod(v*50, 1);

	float m = 1;

	if(tx > t1 && tx < t2) {
		if(ty < t0 || ty > t3) {
			m = 0;
		} else if(ty < t1) {
			m = (ty-t0) / (t1-t0);
		} else if (ty > t2 && ty < t3) {
			m = (t3-ty) / (t3-t2);
		}
		c = Vector(m,m,m);
	} else if(tx > t0 && tx <= t1) {
		if(ty < t0 || ty > t3) {
			m = 0;
		} else if(ty < t1) {
			m = (ty-t0) / (t1-t0);
		} else if (ty > t2 && ty < t3) {
			m = (t3-ty) / (t3-t2);
		}
		c = Vector(m,m,m) * ((tx-t0) / (t1-t0));
	} else if(tx >= t2 && tx <= t3) {
		if(ty < t0 || ty > t3) {
			m = 0;
		} else if(ty < t1) {
			m = (ty-t0) / (t1-t0);
		} else if (ty > t2 && ty < t3) {
			m = (t3-ty) / (t3-t2);
		}
		c = Vector(m,m,m) * ((t3-tx) / (t3-t2));
	}

	return c;
}


inline Vector getBackground(Vector &D) {
	// x = cosf(t)*cosf(p)
	// y = cosf(t)*sinf(p)
	// z = sinf(t)
	float p = atan(D.y / D.x);
	if(p < 0.0) p += 2 * M_PI;

	float t = acos(D.z);

	float u = p * M_1_PI * 0.5f;
	float v = t * M_1_PI * 0.5f;

	if(v < 0.1f) {
		v = 1;
	} else if(v < 0.3) {
		v = (0.3f-v) / 0.2f;
	} else {
		v = 0;
	}

	if(v > 0.04)
		return Vector(.2f,.25f,.4f) * (1-v) + Vector(1,0.92f,0.5f) * (v);
	return Vector(.2f,.25f,.4f);
}

Spectrum Scene::Trace(Ray ray, int depth)
{
	if(depth > maxDepth) return Background;

	Object3d* o = NULL;
	Intersection intersection;
	bool intersected = false;


	// Intersect ray with scene geometry
	for(int i = 0; i < nObjects; i++)
	{
		if(objects[i]->Intersect(ray,intersection))
			intersected = true;
	}

	// If ray hits nothing, return background color
	if(!intersected || !intersection.object) {
		return Background;
	}

	o = intersection.object;
	if(o->transform)
		intersection = o->transform->TransformIntersection(intersection);
	Vector point = intersection.point, normal = intersection.normal.normal();

	bool inside = false;
	float n_dot_d = Vector::dotProduct(normal, ray.D);
	if(n_dot_d > 0)
	{
		normal = normal * -1;
		n_dot_d = -n_dot_d;
		inside = true;
	}

	Spectrum totalLight, diffuse, ambient, Li;
	if(o->material->Ka) ambient = o->material->Ka->eval(intersection);
	if(o->material->Kd) diffuse = o->material->Kd->eval(intersection);
	if(!diffuse.isBlack())
	{
		for(int i = 0; i < nLights; i++)
		{
			Light* light = lights[i];
			Vector lightD = (light->origin() - point);
			float distance = lightD.magnitude();

			if(distance < EPSILON) continue;

			float ld_dot_n = Vector::dotProduct(lightD, normal);
		
			if(ld_dot_n < 0) {
				//continue;
				ld_dot_n = -ld_dot_n;
			}

			// Shadow Testing
			Ray shadowRay(point, lightD.normal(), EPSILON*10, distance);

			bool shadowed = false;
			for(int j = 0; j < nObjects && !shadowed; j++)
			{
				if( objects[j]->IntersectP(shadowRay) )
					shadowed = true;
			}

			// If this light isn't shadowed, add to total lighting contribution
			if(!shadowed) {
				totalLight.addWeighted(ld_dot_n*powf(distance,-2.f),light->Le);
			}
		}
		Li += diffuse * totalLight;
	}

	Spectrum transmitted;
	if(o->material->transmit) transmitted = o->material->transmit->eval(intersection);
	Spectrum reflected;
	if(o->material->mirror) reflected = o->material->mirror->eval(intersection);
	if(!transmitted.isBlack()) {
		float eta_r;
		if(inside) {
			eta_r = o->material->ior / VACUUM_IOR;
		} else {
			eta_r = VACUUM_IOR / o->material->ior;
		}

		float disc = 1 - (eta_r*eta_r) * (1 - n_dot_d*n_dot_d);
		Vector transmitD, reflectD = (ray.D + normal * (-2 * n_dot_d)).normal();

		if(disc < 0)
		{
			transmitD = reflectD;
			Ray transmittedRay(point, transmitD);
			Li += Trace(transmittedRay, depth+1) * transmitted;
			total_internal_reflections++;
		} else {
			transmitD = (ray.D * eta_r - normal * (eta_r * n_dot_d + SQRT(disc))).normal();

			float R0 = powf((1 - eta_r) / (1 + eta_r), 2.0f);
			float alpha = 1.0f + (inside ? n_dot_d : Vector::dotProduct(transmitD, normal));
			float Re = R0 + (1-R0)*alpha*alpha*alpha*alpha*alpha;
			float Tr = 1 - Re, P = 0.25f + 0.5f * Re, RP = Re / P, TP = Tr / (1-P);

			Spectrum Lr;

#ifdef _MONTECARLO
			if(depth > 1) {
				if(FRAND() < P) {
					Ray reflectedRay(point, reflectD);
					Lr.addWeighted(RP,Trace(reflectedRay, depth+1));
				} else {
					Ray transmittedRay(point, transmitD);
					Lr.addWeighted(TP,Trace(transmittedRay, depth+1));
				}
			} else {
#endif
				Ray reflectedRay(point, reflectD);
				Ray transmittedRay(point, transmitD);
				Lr.addWeighted(Re,Trace(reflectedRay, depth+1));
				Lr.addWeighted(Tr,Trace(transmittedRay, depth+1));
#ifdef _MONTECARLO
			}
#endif

			Li += Lr * transmitted;
		}
	}
	if(!reflected.isBlack()) {
		Vector mirrorD = (ray.D + normal * (-2 * n_dot_d)).normal();
		Ray mirrorRay(point, mirrorD);
		Li += Trace(mirrorRay, depth+1) * reflected;
	}

	// Monte-Carlo
#ifdef _MONTECARLO
	Vector sampleDir;
	Spectrum f;
	o->material->generateSample(f, sampleDir, ray.D, intersection);
	if(!f.isBlack()) {
		Ray sampleRay(point, sampleDir);
		Li += Trace(sampleRay, depth+1) * (f * Vector::absDotProduct(sampleDir, normal));
	}
#endif

	return Li + ambient;
}

Scene::~Scene(void)
{
	if(objects) {
		for(int i = 0; i < nObjects; i++)
			delete objects[i];
		delete[] objects;
	}
	if(lights) {
		for(int i = 0; i < nLights; i++)
			delete lights[i];
		delete[] lights;
	}
	for(vector<Material*>::iterator it = materials.begin(); it < materials.end(); it++) {
		delete *it;
	}
	if(camera)
		delete camera;
}
