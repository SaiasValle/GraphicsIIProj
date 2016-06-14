#pragma once
#include "Camera.h"

class Model
{
private:
	Object ObjModel;
	XMFLOAT4X4 WorldMatrix;
	ID3D11Buffer *Vertbuffer = nullptr;
	ID3D11Buffer *Indexbuffer = nullptr;
	string fileName;
	unsigned int numIndices = 0;

public:
	Model();
	~Model();

	XMFLOAT4X4 GetWorldMatrix() { return WorldMatrix; }
	void SetWorldMatrix(XMFLOAT4X4 rhs) { WorldMatrix = rhs; }

	void LoadFromFile();
	void Draw();
};

