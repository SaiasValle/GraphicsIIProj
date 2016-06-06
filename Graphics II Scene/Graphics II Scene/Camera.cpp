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
	int x = (BACKBUFFER_WIDTH / 2), y = (BACKBUFFER_HEIGHT / 2);
	POINT mousePos;
	GetCursorPos(&mousePos);
	SetCursorPos(x, y);

	float mouseDiff[2] = { float(x - mousePos.x), float(y - mousePos.y) };
#if 0
	//mouseDiff[0] = 2 * (float)mousePos.x / (float)BACKBUFFER_WIDTH - 1;
	//mouseDiff[1] = 2 * (float)mousePos.y / (float)BACKBUFFER_HEIGHT - 1;

	XMFLOAT3 lookForward = { 0, 0, 0 };
	lookForward.x = sin(m_CameraMat._31);
	lookForward.z = cos(m_CameraMat._31);
	lookForward.y = sin(m_CameraMat._33);

	XMFLOAT3 campos = { m_CameraMat._31, m_CameraMat._32, m_CameraMat._33 };
	lookForward.x += campos.x;
	lookForward.y += campos.y;
	lookForward.z += campos.z;
	XMVECTOR temp = XMLoadFloat3(&lookForward);
	//temp -= XMLoadFloat3(&campos);

	XMVECTOR tmp = { 0, 1, 0 };
	XMMATRIX WorldtoView = XMLoadFloat4x4(&m_CameraMat);

	//WorldtoView = XMMatrixLookAtLH(XMLoadFloat3(&campos), temp, tmp);
	//XMStoreFloat4x4(&m_CameraMat, WorldtoView);  
#endif

	// Up & Down
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		XMMATRIX rotate = XMMatrixRotationX(mouseDiff[1]*0.0003f);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), rotate));
	}
	// Right & Left
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		XMMATRIX rotate = XMMatrixRotationY(mouseDiff[0]*0.0003f);
		XMStoreFloat4x4(&m_CameraMat, XMMatrixMultiply(XMLoadFloat4x4(&m_CameraMat), rotate));

	}
}

