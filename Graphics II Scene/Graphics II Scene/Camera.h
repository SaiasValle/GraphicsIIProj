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

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
#define MOUSE_LOOK_SPEED_MODIFIER 0.0025f

// Camera Control class
class Camera
{
	// m_CameraMat
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

struct Scene
{
	XMFLOAT4X4 ViewMatrix;
	XMFLOAT4X4 ProjectMatrix;
};
struct Object
{
	XMFLOAT4X4 WorldMatrix;
};

void MouseLook(XMFLOAT4X4 &mat, const float mouseDiff[2], float fTime);
#endif