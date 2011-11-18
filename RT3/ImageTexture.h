#pragma once
#include "Texture.h"
#include "Image.h"

class ImageTexture : public Texture<Spectrum>
{
	Image *image;
	const float s;
	public:
		ImageTexture(const char* fname, Transform* t, float scale = 1.f):s(scale/255.0f),Texture(t)
		{
			image = new Image(fname);
			if(!image->valid)
			{
				delete image;
				throw "Error reading image";
			}
		}
		Spectrum eval(const Intersection& intersection) const
		{
			Vector p(intersection.u*image->w, (intersection.v)*image->h,0);
			if(transform) p = transform->TransformPoint(p);
			int x = MOD((int)p.x, image->w);
			int y = image->h - MOD((int)p.y, image->h);
			unsigned char rgb[3];
			image->getPixel(x,y,rgb);
			float c[3] = {rgb[0] * s, rgb[1] * s, rgb[2] * s};
			return Spectrum(c);
		}
		string toString() const
		{
			return "ImageTexture";
		}
};