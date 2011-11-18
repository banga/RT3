#pragma once

// 24-bit image 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class Image
{
public:
	unsigned char* data;
	long h, w, scanline, size;
	bool valid;

	Image(int width, int height)
	{
		h = height;
		w = width;
		scanline = w*3;
		if(scanline & 3)
			scanline = ((((scanline) >> 2) + 1) << 2);
		size = scanline*h;
		data = new unsigned char[size];
		if(!data)
		{
			valid = false;
			return;
		}
		memset(data, 0, size);
		unsigned char white[3] = {200,200,200};
		for(int i = 0; i < width; i++)
			for(int j = 0; j < height; j++)
			{
				if((i & 16) ^ (j & 16))
					setPixel(i,j,white);
			}
		valid = true;
	}

	// Read PPM file
	Image(const char* fname)
	{
		data = NULL;

		valid = false;
		FILE *fp;
		if(fopen_s(&fp, fname, "rb"))
			return;

		char buf[256];
		fscanf_s(fp,"%c%c",buf,1,buf+1,1);
		buf[2] = 0;
		if(strncmp(buf,"P6",2))
			return;

		int maxgray;
		fscanf_s(fp,"%ld %ld %d\n",&w,&h,&maxgray);

		scanline = w*3;
		if(scanline & 3)
			scanline = ((((scanline) >> 2) + 1) << 2);
		size = scanline*h;
		data = new unsigned char[size];
		if(!data) return;

		size_t count;
		unsigned char *dataline = data+(h-1)*scanline;

		for(long l = 0; l < h; l++)
		{
			count = fread(dataline,1,w*3,fp);
			if(count != w*3)
			{
				delete[] data;
				data = NULL;
				fclose(fp);
				return;
			}
			dataline = dataline - scanline;
		}
		fclose(fp);
		valid = true;
	}

	bool saveAs(const char *fname)
	{
		if(!valid) return false;
		FILE *fp;
		if(fopen_s(&fp, fname, "wb"))
			return false;

		fprintf_s(fp,"P6 %ld %ld %d\n", w, h, 255);

		unsigned char *dataline = data+(h-1)*scanline;
		for(long l = 0; l < h; l++)
		{
			size_t count = fwrite(dataline,1,w*3,fp);
			if(count != w*3)
			{
				fclose(fp);
				return false;
			}
			dataline = dataline - scanline;
		}
		fclose(fp);
		return true;
	}

	void resize(int width, int height)
	{
		if(w == width && h == height)
			return;
		if(data)
			delete[] data;

		h = height;
		w = width;
		scanline = w*3;
		if(scanline & 3)
			scanline = ((((scanline) >> 2) + 1) << 2);
		size = scanline*h;
		data = new unsigned char[size];
		if(!data)
		{
			valid = false;
			return;
		}
		memset(data, 0, size);
		unsigned char white[3] = {200,200,200};
		for(int i = 0; i < width; i++)
			for(int j = 0; j < height; j++)
			{
				if((i & 16) ^ (j & 16))
					setPixel(i,j,white);
			}
		valid = true;
	}

	void setPixel(const int x, const int y, unsigned char rgb[3])
	{
		memcpy(data + (x*3) + (h-1-y)*scanline, rgb, 3);
	}

	void getPixel(const int x, const int y, unsigned char rgb[3]) const
	{
		memcpy(rgb, data + (x*3) + (h-1-y)*scanline, 3);
	}

	~Image()
	{
		if(data)
			delete[] data;
	}
};