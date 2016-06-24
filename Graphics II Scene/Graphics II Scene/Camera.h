#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <DirectXMath.h>
#include <windows.h>
#include <queue>
#include <iostream>
#include <ctime>
#include <assert.h>
using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3d11.lib")
#include <d3d11.h>

#define BACKBUFFER_WIDTH	1000
#define BACKBUFFER_HEIGHT	1000
#define MOUSE_SPEED_MODIFIER 0.0003f
#define MOUSE_MOVE_MODIFIER 0.04f

// Camera Control class
class Camera
{
private:
	// 4x4 Matrix representing camera position and orientation in worldspace
	XMFLOAT4X4	m_CameraMat;

public:

	// Constructor
	Camera();
	// Destructor
	~Camera(void);
	// Getters & Mutators
	XMFLOAT4X4 GetCameraMat() { return m_CameraMat; }
	void SetCameraMat(XMFLOAT4X4 cameramat) { m_CameraMat = cameramat; }

	void MoveCamera();
	void RotateCamera();

};

struct Object
{
	XMFLOAT4X4 WorldMatrix;
};
struct Scene
{
	XMFLOAT4X4 ViewMatrix;
	XMFLOAT4X4 ProjectMatrix;
};

void MouseLook(XMFLOAT4X4 &mat, const float mouseDiff[2], float fTime);
#endif