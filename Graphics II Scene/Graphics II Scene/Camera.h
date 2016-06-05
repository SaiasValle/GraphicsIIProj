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

	void Update(float fTime);

	// ApplyCameraTransform
	void ApplyCameraTransform();
};

void MouseLook(XMFLOAT4X4 &mat, const float mouseDiff[2], float fTime);
#endif