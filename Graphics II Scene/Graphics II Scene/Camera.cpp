#include "Camera.h"

Camera::Camera()
{
	XMStoreFloat4x4(&m_CameraMat, XMMatrixIdentity());
}
Camera::~Camera()
{

}

void Camera::MoveCamera()
{
	if (GetAsyncKeyState('E'))
	{
		XMVECTOR move{ 0, MOUSE_MOVE_MODIFIER, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('Q'))
	{
		XMVECTOR move = { 0, -MOUSE_MOVE_MODIFIER, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('W'))
	{
		XMVECTOR move = { 0, 0, MOUSE_MOVE_MODIFIER, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('S'))
	{
		XMVECTOR move = { 0, 0, -MOUSE_MOVE_MODIFIER, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('D'))
	{
		XMVECTOR move = { MOUSE_MOVE_MODIFIER, 0, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
	}
	if (GetAsyncKeyState('A'))
	{
		XMVECTOR move = { -MOUSE_MOVE_MODIFIER, 0, 0, 1 };
		XMMATRIX trans = XMMatrixTranslationFromVector(move);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(trans, XMLoadFloat4x4(&m_CameraMat)));
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

	int x = (BACKBUFFER_WIDTH / 2), y = (BACKBUFFER_HEIGHT / 2);
	GetCursorPos(&mousePos);
	SetCursorPos(x, y);
	float mouseDiff[2] = { float(mousePos.x - x), float(mousePos.y - y) };
	
	if (GetAsyncKeyState(VK_RBUTTON))
	{
#if 1
		XMMATRIX camera_mat = XMLoadFloat4x4(&m_CameraMat);
		XMVECTOR pos;
		pos.m128_f32[0] = camera_mat.r[3].m128_f32[0];
		pos.m128_f32[1] = camera_mat.r[3].m128_f32[1];
		pos.m128_f32[2] = camera_mat.r[3].m128_f32[2];

		camera_mat.r[3] = XMVectorSet(0, 0, 0, 1);

		// Up & Down
		XMMATRIX rotate = XMMatrixRotationX(mouseDiff[1] * MOUSE_SPEED_MODIFIER);
		camera_mat = XMMatrixMultiply(rotate, camera_mat);

		// Right & Left
		rotate = XMMatrixRotationY(mouseDiff[0] * MOUSE_SPEED_MODIFIER);
		camera_mat = XMMatrixMultiply(camera_mat, rotate);

		camera_mat.r[3].m128_f32[0] = pos.m128_f32[0];
		camera_mat.r[3].m128_f32[1] = pos.m128_f32[1];
		camera_mat.r[3].m128_f32[2] = pos.m128_f32[2];

		XMStoreFloat4x4(&m_CameraMat, camera_mat);
	}
#endif

#if 0
	XMFLOAT4X4 camera_mat = m_CameraMat;
	XMFLOAT4X4 newcamera_mat;
	XMFLOAT3 xaxis, yaxis, zaxis;
	XMFLOAT3 normX, normY, normZ;

	// Up & Down
	XMMATRIX rotate = XMMatrixRotationX(mouseDiff[1] * 0.0003f);
	XMStoreFloat4x4(&camera_mat, XMMatrixMultiply(XMLoadFloat4x4(&camera_mat), rotate));

	// Right & Left
	rotate = XMMatrixRotationY(mouseDiff[0] * 0.0003f);
	XMStoreFloat4x4(&camera_mat, XMMatrixMultiply(XMLoadFloat4x4(&camera_mat), rotate));

	// Rebuild Matrix

	// Z-axis
	zaxis = { camera_mat._31, camera_mat._32, camera_mat._33 };

	XMStoreFloat3(&normZ, XMVector3Normalize(XMLoadFloat3(&zaxis)));
	camera_mat._31 = normZ.x;
	camera_mat._32 = normZ.y;
	camera_mat._33 = normZ.z;
	// Temp Y-axis
	yaxis = { 0, 1, 0 };

	// X-axis
	XMStoreFloat3(&xaxis, XMVector3Cross(XMLoadFloat3(&yaxis), XMLoadFloat3(&zaxis)));

	XMStoreFloat3(&normX, XMVector3Normalize(XMLoadFloat3(&xaxis)));
	camera_mat._11 = normX.x;
	camera_mat._12 = normX.y;
	camera_mat._13 = normX.z;

	// Real Y-axis
	XMStoreFloat3(&yaxis, XMVector3Cross(XMLoadFloat3(&zaxis), XMLoadFloat3(&xaxis)));

	XMStoreFloat3(&normY, XMVector3Normalize(XMLoadFloat3(&yaxis)));
	camera_mat._21 = normY.x;
	camera_mat._22 = normY.y;
	camera_mat._23 = normY.z;

	m_CameraMat = camera_mat;
	}
#endif
}

