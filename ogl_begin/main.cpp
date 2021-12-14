#include "Camera.h"
#include <mmsystem.h>
#include "irrKlang.h"
using namespace irrklang;
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "irrKlang.lib")
#pragma comment(lib, "winmm.lib")
#define PUSH glPushMatrix()
#define POP glPopMatrix()
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
ISoundEngine* SoundEngine = createIrrKlangDevice();
float kube[] = { 0,0,0, 0,1,0, 1,1,0, 1,0,0, 0,0,1, 0,1,1, 1,1,1, 1,0,1 };
GLuint kubeInd[] = { 0,1,2, 2,3,0, 4,5,6, 6,7,4, 3,2,5, 6,7,3, 0,1,5,
					5,4,0, 1,2,6, 6,5,1, 0,3,7, 7,4,0 };
BOOL showMask = FALSE;

typedef struct {
	float r, g, b;
}TColor;
typedef struct {
	TColor clr;
}TCell;
#define pW 40
#define pH 40
TCell map[pW][pH];

void Map_Init()
{
	for (int i = 0; i < pW; i++)
		for (int j = 0; j < pH; j++)
		{
			float dc = (rand() % 20) * 0.01;
			map[i][j].clr.r = 0.31 + dc;
			map[i][j].clr.g = 0.6 + dc;
			map[i][j].clr.b = 0.13 + dc;
		}
}

void WndResize(int x, int y)
{
	glViewport(0, 0, x, y);// размер области вывода
	float k = x / (float)y;
	float sz = 0.1;
	glLoadIdentity();
	glFrustum(-k * sz, k * sz, -sz, sz, sz * 2, 100); // установка перспективной проекции
}

#define enemyCnt 40
struct {
	float x, y, z;
	BOOL active;
}enemy[enemyCnt];

void Enemy_Init()
{
	for (int i = 0; i < enemyCnt; i++)
	{
		enemy[i].active = TRUE;
		enemy[i].x = rand() % pW;
		enemy[i].y = rand() % pH;
		enemy[i].z = rand() % 5;
	}
}

void Enemy_Show()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, kube);
	for (int i = 0; i < enemyCnt; i++)
	{
		if (!enemy[i].active) continue;
		PUSH;
		glTranslatef(enemy[i].x, enemy[i].y, enemy[i].z);
		if (showMask)
			glColor3ub(255 - i, 0, 0);
		else
			glColor3ub(244, 60, 43);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, kubeInd);
		POP;
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}




void Game_Move()
{
	camera.Camera_MoveDirection(0.05);
	camera.Camera_AutoMoveByMouse(400, 300, 0.2);
}

void Game_Init()
{	
	SoundEngine->play2D("breakout.wav", true);
	glEnable(GL_DEPTH_TEST);
	Map_Init();
	Enemy_Init();
	RECT rct;
	GetClientRect(hwnd, &rct);
	WndResize(rct.right, rct.bottom);
}

void Game_Show()
{
	
	if (showMask) glClearColor(0, 0, 0, 0);
	glClearColor(0.6, 0.8, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	PUSH;
	camera.Camera_Apply();
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, kube);
	for (int i = 0; i < pW; i++)
		for (int j = 0; j < pH; j++)
		{
			PUSH;
			glTranslatef(i, j, 0);
			if (showMask)
				glColor3f(0, 0, 0);
			else
				glColor3f(map[i][j].clr.r, map[i][j].clr.g, map[i][j].clr.b);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, kubeInd);
			POP;
		}
	glDisableClientState(GL_VERTEX_ARRAY);

	Enemy_Show();
	POP;
}

void Player_Shoot()
{
	showMask = TRUE;
	Game_Show();
	showMask = FALSE;

	RECT rct;
	GLubyte clr[3];
	GetClientRect(hwnd, &rct);
	glReadPixels(rct.right / 2.0, rct.bottom / 2.0, 1, 1,
		GL_RGB, GL_UNSIGNED_BYTE, clr);
	if (clr[0] > 0)
		enemy[255 - clr[0]].active = FALSE;
	PlaySound("shoot.wav", NULL, SND_ASYNC);
}


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;
	float theta = 0.0f;

	/* register window class */
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "GLSample";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		"GLSample",
		"OpenGL Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,
		800,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, nCmdShow);

	/* enable OpenGL for the window */
	EnableOpenGL(hwnd, &hDC, &hRC);

	Game_Init();

	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			/* OpenGL animation code goes here */
			
			Game_Move();
			Game_Show();

			/*glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glPushMatrix();
			glRotatef(theta, 0.0f, 0.0f, 1.0f);

			glBegin(GL_TRIANGLES);

			glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f, 1.0f);
			glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f, -0.5f);
			glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);

			glEnd();

			glPopMatrix();*/

			SwapBuffers(hDC);

			/*theta += 1.0f;*/
			Sleep(1);
		}
	}

	/* shutdown OpenGL */
	DisableOpenGL(hwnd, hDC, hRC);

	/* destroy the window explicitly */
	DestroyWindow(hwnd);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		WndResize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_SETCURSOR:
		ShowCursor(FALSE);
		break;
	case WM_LBUTTONDOWN:
		Player_Shoot();
		break;
	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
	break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}