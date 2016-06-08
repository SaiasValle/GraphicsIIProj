#include "Camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::MoveCamera()
{
	if (GetAsyncKeyState('W') && GetAsyncKeyState(VK_SHIFT))
	{
		XMVECTOR move{ 0, -0.001f, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), trans));
	}
	if (GetAsyncKeyState('S') && GetAsyncKeyState(VK_SHIFT))
	{
		XMVECTOR move = { 0, 0.001f, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), trans));
	}
	if (GetAsyncKeyState('W'))
	{
		XMVECTOR move = { 0, 0, -0.001f, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), trans));
	}
	if (GetAsyncKeyState('S'))
	{
		XMVECTOR move = { 0, 0, 0.001f, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), trans));
	}
	if (GetAsyncKeyState('D'))
	{
		XMVECTOR move = { -0.001f, 0, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), trans));
	}
	if (GetAsyncKeyState('A'))
	{
		XMVECTOR move = { 0.001f, 0, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), trans));
	}
	// Reset Camera to Origin
	if (GetAsyncKeyState(VK_BACK))
	{
		XMStoreFloat4x4(&m_CameraMat, XMMatrixTranslation(0.0f, 0.0f, 5.0f));
	}
}
void Camera::RotateCamera()
{
	POINT mousePos;
	XMFLOAT4X4 camera_mat = m_CameraMat;
	XMFLOAT4X4 newcamera_mat;
	XMFLOAT3 xaxis, yaxis, zaxis;
	XMFLOAT3 normX, normY, normZ;

	int x = (BACKBUFFER_WIDTH / 2), y = (BACKBUFFER_HEIGHT / 2);
	GetCursorPos(&mousePos);
	SetCursorPos(x, y);
	float mouseDiff[2] = { float(x - mousePos.x), float(y - mousePos.y) };
	
	if (GetAsyncKeyState(VK_RBUTTON))
	{
#if 0
  
		// Up & Down
		XMMATRIX rotate = XMMatrixRotationX(mouseDiff[1] * 0.0003f);
		camera_mat = XMMatrixMultiply(camera_mat, rotate);

		// Right & Left
		rotate = XMMatrixRotationY(mouseDiff[0] * 0.0003f);
		camera_mat = XMMatrixMultiply(camera_mat, rotate);

		// Rebuild Matrix
		newcamera_mat = camera_mat;
		// Z-axis
		zaxis = camera_mat.r[3];
		newcamera_mat.r[3] = zaxis;
		newcamera_mat.r[3] = XMVector3Normalize(zaxis);
		// Temp Y-axis
		yaxis = { 0, 1, 0 };
		// X-axis
		xaxis = XMVector3Cross(yaxis, zaxis);
		newcamera_mat.r[0] = xaxis;
		newcamera_mat.r[0] = XMVector3Normalize(xaxis);
		// Real Y-axis
		yaxis = XMVector3Cross(zaxis, xaxis);
		newcamera_mat.r[2] = yaxis;
		newcamera_mat.r[2] = XMVector3Normalize(yaxis);

		XMStoreFloat4x4(&m_CameraMat, newcamera_mat);
	}
#endif

#if 1
	// Up & Down
	XMMATRIX rotate = XMMatrixRotationX(mouseDiff[1] * 0.0003f);
	XMStoreFloat4x4(&camera_mat, XMMatrixMultiply(XMLoadFloat4x4(&camera_mat), rotate));

	// Right & Left
	rotate = XMMatrixRotationY(mouseDiff[0] * 0.0003f);
	XMStoreFloat4x4(&camera_mat, XMMatrixMultiply(XMLoadFloat4x4(&camera_mat), rotate));

	// Rebuild Matrix
	newcamera_mat = camera_mat;

	// Z-axis
	zaxis = { camera_mat._31, camera_mat._32, camera_mat._33 };
	camera_mat._31 = zaxis.x;
	camera_mat._32 = zaxis.y;
	camera_mat._33 = zaxis.z;
	XMStoreFloat3(&normZ, XMVector3Normalize(XMLoadFloat3(&zaxis)));
	camera_mat._31 = normZ.x;
	camera_mat._32 = normZ.y;
	camera_mat._33 = normZ.z;
	// Temp Y-axis
	yaxis = { 0, 1, 0 };

	// X-axis
	XMStoreFloat3(&xaxis, XMVector3Cross(XMLoadFloat3(&yaxis), XMLoadFloat3(&zaxis)));
	camera_mat._11 = xaxis.x;
	camera_mat._12 = xaxis.y;
	camera_mat._13 = xaxis.z;
	XMStoreFloat3(&normX, XMVector3Normalize(XMLoadFloat3(&xaxis)));
	camera_mat._11 = normX.x;
	camera_mat._12 = normX.y;
	camera_mat._13 = normX.z;

	// Real Y-axis
	XMStoreFloat3(&yaxis, XMVector3Cross(XMLoadFloat3(&zaxis), XMLoadFloat3(&xaxis)));
	camera_mat._21 = yaxis.x;
	camera_mat._22 = yaxis.y;
	camera_mat._23 = yaxis.z;
	XMStoreFloat3(&normY, XMVector3Normalize(XMLoadFloat3(&yaxis)));
	camera_mat._21 = normY.x;
	camera_mat._22 = normY.y;
	camera_mat._23 = normY.z;

	m_CameraMat = camera_mat;
	}
#endif
}

