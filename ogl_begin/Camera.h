#pragma once
#include <windows.h>
#include <gl/gl.h>
#include <math.h>
HWND hwnd;
#define PI 3.1415926535
class Camera
{
	float x, y, z;
	float Xrot, Zrot;
public:
	Camera()
	{
		x = 0;
		y = 0;
		z = 1.7;
		Xrot = 70;
		Zrot = -40;
	}
	void Camera_Apply()
	{
		glRotatef(-Xrot, 1, 0, 0);
		glRotatef(-Zrot, 0, 0, 1);
		glTranslatef(-x, -y, -z);
	}

	void Camera_Rotation(float xAngle, float zAngle)
	{
		Zrot += zAngle;
		if (Zrot < 0) Zrot += 360;
		if (Zrot > 360) Zrot -= 360;
		Xrot += xAngle;
		if (Xrot < 0) Xrot = 0;
		if (Xrot > 180) Xrot = 180;
	}

	void Camera_AutoMoveByMouse(int centreX, int centreY, float speed)
	{
		POINT cur;
		static POINT base = { centreX,centreY };
		GetCursorPos(&cur);
		Camera_Rotation((base.y - cur.y) * speed, (base.x - cur.x) * speed);
		SetCursorPos(base.x, base.y);
	}
	void Camera_MoveDirection(float ispeed)
	{
		if (GetForegroundWindow() != hwnd) return;

		float ugol = -Zrot / 180 * PI;
		float speed = 0;
		if (GetKeyState('W') < 0) speed = ispeed;
		if (GetKeyState('S') < 0) speed = -ispeed;
		if (GetKeyState('A') < 0)
		{
			speed = ispeed; ugol -= PI * 0.5;
		}
		if (GetKeyState('D') < 0)
		{
			speed = ispeed; ugol += PI * 0.5;
		}
		if (speed != 0)
		{
			x += sin(ugol) * speed;
			y += cos(ugol) * speed;
		}		
	}
}camera;
