// RT3.cpp : Entry point for the Raytracer
#include "stdafx.h"
#include "OpenGL.h"
#include "image.h"
#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "Mesh.h"
#include "PerspectiveCamera.h"
#include "PointLight.h"
#include "Parser.h"

float theta, phi, r;
int xTiles = 16, yTiles = 8;
const int BITS = 32;

clock_t renderTime;
HANDLE hStdOut = INVALID_HANDLE_VALUE;

HANDLE hRenderThread = NULL;
bool rendering;

void (*onRenderComplete) ();

DWORD WINAPI renderThread(LPVOID lpParam)
{
	rendering = true;
	renderTime = clock();

	scene->Render();

	rendering = false;
	return 0;
}

void abortRender()
{
	if(hRenderThread != NULL)
	{
		scene->abortRender();
		WaitForSingleObject(hRenderThread,INFINITE);
		CloseHandle(hRenderThread);
		hRenderThread = NULL;
		printf("\nRender aborted");
	}
}

void stopRender()
{
	if(hRenderThread != NULL)
	{
		WaitForSingleObject(hRenderThread,INFINITE);
		CloseHandle(hRenderThread);
		hRenderThread = NULL;
	}
}

void doRender()
{
	// /*
	if(rendering)
	{
		abortRender();
	}
	else
	{
		stopRender();
	}
// /*
	if((hRenderThread = CreateThread(NULL,0,renderThread,NULL,0,NULL)) == NULL)
	{
		printf("\nError creating render thread.");
		return;
	} //	*/
 /*
	rendering = true;
	renderTime = clock();
	scene->Render();
	rendering = false; // */
}

void doRenderComplete()
{
	rendering = false;
	char str[45];
	memset(str,'.',45); str[40] = 0;

	float dt = (clock() - renderTime) / ((float)CLOCKS_PER_SEC), dt_rem = dt;

	printf("\rRendering%s%*.2f%% (%5.2fs|%5.2fs)",
		str, 5, 100.f, dt, dt_rem);

	if(!scene->image->saveAs("out.ppm"))
	{
		printf("\nError writing out.ppm");
	}
	else
	{
		printf("\nOut.ppm saved successfully");
	}
}

void initScene()
{
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glDrawPixels(Width, Height, GL_RGB, GL_UNSIGNED_BYTE, scene->image->data);

	if(rendering) {
		float r = scene->pixelsTraced / ((float)Width*Height);
		int w = (int)(40 * r);

		char str[45];
		memset(str,'.',45); str[w] = 0;

		float dt = (clock() - renderTime) / ((float)CLOCKS_PER_SEC), dt_rem = ((r > 0) ? (dt / r) : 0);

		printf("\rRendering%s%*.2f%% (%5.2fs|%5.2fs)",
			str, 45-w, 100 * r, dt, dt_rem);
	}
}

void destroyScene()
{
	if(scene) {
		abortRender();
		delete(scene);
	}
}

void drawConsole()
{
	if(hStdOut != INVALID_HANDLE_VALUE)
	{
		COORD coords;
		coords.X = 0;
		coords.Y = 0;
		SetConsoleCursorPosition(hStdOut,coords);
		SetConsoleTextAttribute(hStdOut,BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED|BACKGROUND_INTENSITY);
		DWORD n;
		FillConsoleOutputCharacter(hStdOut,' ',6400,coords,&n);
	}
	printf("Width: %4d Height: %4d spp: %4d \n", Width, Height, samplesPerPixel);
	if(hStdOut != INVALID_HANDLE_VALUE)
	{
		SetConsoleTextAttribute(hStdOut,FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	}
}

void doResize(int width, int height)
{
/*	Width = width;
	Height = height;

	if(scene)
	{
		abortRender();
		scene->setImageParams(width,height,xTiles,yTiles);
	}*/
	drawConsole();
}

void ResizeWindowClient(int nWidth, int nHeight)
{
	RECT rcClient, rcWindow;
	POINT ptDiff;
    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWindow);
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	MoveWindow(hWnd,rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}

void doKeyUp(WPARAM wParam)
{
	switch(wParam)
	{
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_ADD:
		case VK_SUBTRACT:
			break;
		case 'M':
			int w, h;
			printf("\nEnter new width:");
			scanf_s("%d",&w);
			printf("\nEnter new height:");
			scanf_s("%d",&h);
			ResizeWindowClient(w,h);
			doResize(w,h);
			break;
		case VK_SPACE:
			doRender();
			break;
		case VK_RETURN:
			abortRender();
			break;
	}
}

void doKeyDown(WPARAM wParam)
{
	bool changed = true;
	switch(wParam)
	{
		case 0xDB:
			samplesPerPixel--;
			if(samplesPerPixel <= 0)
				samplesPerPixel = 1;
			abortRender();
			scene->setSamples(samplesPerPixel);
			drawConsole();
			break;
		case 0xDD:
			samplesPerPixel++;
			abortRender();
			scene->setSamples(samplesPerPixel);
			drawConsole();
			break;
		case VK_UP:
			theta-=0.05f;
			break;
		case VK_DOWN:
			theta+=0.05f;
			break;
		case VK_LEFT:
			phi-=0.05f;
			break;
		case VK_RIGHT:
			phi+=0.05f;
			break;
		case VK_ADD:
			r-=0.5f;
			break;
		case VK_SUBTRACT:
			r+=0.5f;
			break;
		default:
			changed = false;
			break;
	}
//	if(changed) {
//		doRender();
//	}
}

int main(int argc, char *argv[])
{
	MSG msg;
	bool Done=false;

	char* fname = NULL;
	if(argc > 1)
		fname = argv[1];

	onRenderComplete = doRenderComplete;
	if(!Parse(fname))
	{
		return 0;
	}

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hStdOut == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if(!RTInit())
	{
		return 0;
	}

	 if(!CreateGLWindow(L"RT3",Width,Height,BITS,false))
	 {
		return 0;
	 }

	 doRender();

	// Main message loop:
	while (!Done)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
			{
				Done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(active)
			{
				if(keys[VK_ESCAPE])
				{
					Done = true;
				}
				else
				{
					drawScene();
					SwapBuffers(hDC);
				}
				if (keys[VK_F1])					// Is F1 Being Pressed?
				{
					keys[VK_F1]=FALSE;				// If So Make Key FALSE
					killGLWindow();					// Kill Our Current Window
					fullscreen=!fullscreen;			// Toggle Fullscreen / Windowed Mode
					// Recreate Our OpenGL Window
					if (!CreateGLWindow(L"OpenGL Test",Width,Height,BITS,fullscreen))
					{
						return 0;				// Quit If Window Was Not Created
					}
				}
			}
		}
	}
	killGLWindow();
// */
	destroyScene();
	RTCleanUp();
	system("PAUSE");
	return 0;
}