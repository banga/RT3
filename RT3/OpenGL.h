#include "stdafx.h"
#include <gl\GL.h>
#include <gl\GLU.h>

// Global Variables:
HINSTANCE hInst;
HGLRC hRC=NULL;
HDC hDC=NULL;
HWND hWnd=NULL;

bool keys[256];
bool active=true;
bool fullscreen=false;
bool perspective=false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void initScene();
void destroyScene();
void doKeyUp(WPARAM);
void doKeyDown(WPARAM);
void doResize(int width, int height);

bool initGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
//	glClearDepth(1.0f);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return true;
}

void ReSizeGLScene(GLsizei width, GLsizei height)
{
	if(height==0)
	{
		height=1;
	}
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(perspective)
	{
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	}
	else
	{
		gluOrtho2D(0,(GLfloat)width,0,(GLfloat)height);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void killGLWindow()
{
	if(fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}
	if(hRC)
	{
		if(!wglMakeCurrent(NULL,NULL))
		{
			MessageBox(NULL,L"Release of DC and RC failed",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		if(!wglDeleteContext(hRC))
		{
			MessageBox(NULL,L"Release of RC failed",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;
	}
	if(hDC && !ReleaseDC(hWnd,hDC))
	{
		MessageBox(NULL,L"Release of DC failed",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC = NULL;
	}
	if(hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL,L"Release of hWnd failed",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}
	if(!UnregisterClass(L"RT3",hInst))
	{
		MessageBox(NULL,L"Could not unregister class",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC = NULL;
	}
}

bool CreateGLWindow(LPCWSTR title, const int width, const int height, int bits, bool isFullscreen)
{
	GLuint PixelFormat;
	WNDCLASSEX wcex;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect;

	windowRect.left = (long)0;
	windowRect.right = (long)width;
	windowRect.top = (long)0;
	windowRect.bottom = (long)height;
	fullscreen = isFullscreen;

	hInst = GetModuleHandle(NULL);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= L"RT3";
	wcex.hIconSm		= NULL;

	if(!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,L"Failed to register window class",L"Error",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if(fullscreen)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			fullscreen = false;
		}
	}
	if(fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
	if (!(hWnd=CreateWindowEx(	dwExStyle,	// Extended Style For The Window
					L"RT3",					// Class Name
					title,					// Window Title
					WS_CLIPSIBLINGS |		// Required Window Style
					WS_CLIPCHILDREN |		// Required Window Style
					dwStyle,				// Selected Window Style
					0, 0,					// Window Position
					windowRect.right-windowRect.left,	// Calculate Adjusted Window Width
					windowRect.bottom-windowRect.top,	// Calculate Adjusted Window Height
					NULL,					// No Parent Window
					NULL,					// No Menu
					hInst,  				// Instance
					NULL)))					// Don't Pass Anything To WM_CREATE
	{
		killGLWindow();
		MessageBox(NULL,L"Window creation failed",L"Error",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	static PIXELFORMATDESCRIPTOR pfd=	// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		bits,							// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		16,								// 16Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
	};

	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		killGLWindow();							// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;							// Return FALSE
	}

	if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))
	{
		killGLWindow();
		MessageBox(NULL,L"Can't find suitable pixel format",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if(!(SetPixelFormat(hDC,PixelFormat,&pfd)))
	{
		killGLWindow();
		MessageBox(NULL,L"Can't set the pixel format",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if(!(hRC=wglCreateContext(hDC)))
	{
		killGLWindow();
		MessageBox(NULL,L"Can't create a GL rendering cotnext",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if(!(wglMakeCurrent(hDC,hRC)))
	{
		killGLWindow();
		MessageBox(NULL,L"Can't activate the GL rendering context",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ReSizeGLScene(width,height);

	if(!initGL())
	{
		killGLWindow();
		MessageBox(NULL,L"OpenGL initialization failed",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_ACTIVATE:
		{
			active = (HIWORD(wParam) == 0);
			return 0;
		}
	case WM_SYSCOMMAND:
		{
			switch(wParam)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;
			}
			break;
		}
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_KEYDOWN:
		{
			keys[wParam] = true;
			doKeyDown(wParam);
			return 0;
		}
	case WM_KEYUP:
		{
			keys[wParam] = false;
			doKeyUp(wParam);
			return 0;
		}
	case WM_SIZE:
		{
			doResize(LOWORD(lParam),HIWORD(lParam));
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}