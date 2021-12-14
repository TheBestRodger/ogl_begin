//#include <windows.h>
//#include <gl/gl.h>
//#include "Camera.h"
//#pragma comment(lib, "opengl32.lib")
//struct Camera camera = { 0,0,1.7,70,-40 };
//void Camera_Apply()
//{
//	glRotatef(-camera.Xrot, 1, 0, 0);
//	glRotatef(-camera.Zrot, 0, 0, 1);
//	glTranslatef(-camera.x, -camera.y, -camera.z);
//}
//
//void Camera_Rotation(float xAngle, float zAngle)
//{
//	camera.Zrot += zAngle;
//	if (camera.Zrot < 0) camera.Zrot += 360;
//	if (camera.Zrot > 360) camera.Zrot -= 360;
//	camera.Xrot += xAngle;
//	if (camera.Xrot < 0) camera.Xrot = 0;
//	if (camera.Xrot > 180) camera.Xrot = 180;
//}
//
//void Camera_AutoMoveByMouse(int centreX, int centreY)
//{
//	POINT cur;
//	static POINT base = { centreX,centreY };
//	GetCursorPos(&cur);
//	Camera_Rotation((base.y - cur.y) / 5.0, (base.x - cur.x) / 5.0);
//	SetCursorPos(base.x, base.y);
//}
//void Camera_MoveDirection()
//{
//	if (GetForegroundWindow() != hwnd) return;
//
//	float ugol = -camera.Zrot / 180 * PI;
//	float speed = 0;
//	if (GetKeyState('W') < 0) speed = 0.1;
//	if (GetKeyState('S') < 0) speed = -0.1;
//	if (GetKeyState('A') < 0)
//	{
//		speed = 0.1; ugol -= PI * 0.5;
//	}
//	if (GetKeyState('D') < 0)
//	{
//		speed = 0.1; ugol += PI * 0.5;
//	}
//	if (speed != 0)
//	{
//		camera.x += sin(ugol) * speed;
//		camera.y += cos(ugol) * speed;
//	}
//	Camera_AutoMoveByMouse(400, 300);
//}