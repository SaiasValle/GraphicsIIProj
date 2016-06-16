#pragma once
#include "Camera.h"


class Model
{
private:
	Object ObjModel;
	ID3D11Buffer *Constbuffer = nullptr;
	ID3D11Buffer *Vertbuffer = nullptr;
	ID3D11Buffer *Indexbuffer = nullptr;
	string fileName;
	unsigned int numIndices = 0;
	// Texture
	ID3D11Texture2D *m_texture = nullptr;
	ID3D11ShaderResourceView *m_SRV = nullptr;
	ID3D11DepthStencilView *m_DSV = nullptr;

public:
	Model();
	~Model();

	string GetFileName() { return fileName; }
	void SetFileName(string rhs) { fileName = rhs; }
	//void SetFileNameChar(char *rhs) { fileName.c_str() = rhs; }
	XMFLOAT4X4 GetWorldMatrix() { return ObjModel.WorldMatrix; }
	void SetWorldMatrix(XMFLOAT4X4 rhs) { ObjModel.WorldMatrix = rhs; }
	void TranslateModel(XMFLOAT3 posVector);
	void RotateModel(XMFLOAT3 rotXYZ);
	void ScaleModel(XMFLOAT3 scale);

	void LoadTextureDDS(string textureName, ID3D11Device *device);
	void LoadTextureMTL(string textureName, ID3D11Device *device);
	void LoadFromFile(ID3D11Device *device);
	template<typename Type>
	void Initialize(ID3D11Device *device, ID3D11Buffer **vertbuff, vector<Type> verts, ID3D11Buffer **indexBuff, vector<unsigned int> indices);
	void Draw(ID3D11DeviceContext *device);
};

