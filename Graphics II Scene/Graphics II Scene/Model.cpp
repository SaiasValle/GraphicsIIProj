#include "Model.h"
#include "DEMO_APP.h"

Model::Model()
{
}

Model::~Model()
{
	Vertbuffer->Release();
	Indexbuffer->Release();
}

void Model::LoadFromFile()
{
	vector<XMFLOAT4> position;
	vector<XMFLOAT2> uv;
	vector<XMFLOAT3> normals;
	vector<unsigned int> posIndex;
	vector<unsigned int> uvIndex;
	vector<unsigned int> normIndex;
	vector<DEMO_APP::SIMPLE_VERTEX> verts;
	char buffer[256] = { 0 };
	FILE *file;

	fopen_s(&file, fileName.c_str(), "r");

	if (file != nullptr)
	{
		for (;;)
		{
			int end = fscanf_s(file, "%s", buffer);
			if (end == EOF)
				break;

			if (strcmp(buffer, "v") == 0)
			{
				float x = 0, y = 0, z = 0;
				fscanf_s(file, "%f %f %f\n", &x, &y, &z);
				position.push_back(XMFLOAT4(x, y, z, 1.0f));
			}
			if (strcmp(buffer, "vt") == 0)
			{
				float u = 0, v = 0;
				fscanf_s(file, "%f %f\n", &u, &v);
				uv.push_back(XMFLOAT2(u, v));
			}
			if (strcmp(buffer, "vn") == 0)
			{
				float x = 0, y = 0, z = 0;
				fscanf_s(file, "%f %f %f\n", &x, &y, &z);
				normals.push_back(XMFLOAT3(x, y, z));
			}
			if (strcmp(buffer, "f") == 0)
			{
				unsigned int posI[3], uvI[3], normI[3];
				fscanf_s(file, "%d%d%d %d%d%d %d%d%d\n",
					&posI[0], &uvI[0], &normI[0],
					&posI[1], &uvI[1], &normI[1],
					&posI[2], &uvI[2], &normI[2]);

				posIndex.push_back(posI[0]);
				uvIndex.push_back(uvI[0]);
				normIndex.push_back(normI[0]);

				posIndex.push_back(posI[1]);
				uvIndex.push_back(uvI[1]);
				normIndex.push_back(normI[1]);

				posIndex.push_back(posI[2]);
				uvIndex.push_back(uvI[2]);
				normIndex.push_back(normI[2]);
			}
		}
		vector<unsigned int> index2;

		for (unsigned int i = 0; i < (unsigned int)posIndex.size(); i++)
		{
			DEMO_APP::SIMPLE_VERTEX tempVert;

			tempVert.x = position[posIndex[i] - 1].x;
			tempVert.y = position[posIndex[i] - 1].y;
			tempVert.z = position[posIndex[i] - 1].z;

			tempVert.uvw[0] = uv[uvIndex[i] - 1].x;
			tempVert.uvw[1] = uv[uvIndex[i] - 1].y;

			tempVert.normal[0] = normals[normIndex[i] - 1].x;
			tempVert.normal[1] = normals[normIndex[i] - 1].y;
			tempVert.normal[2] = normals[normIndex[i] - 1].z;

			verts.push_back(tempVert);
			index2.push_back(i);
		}
	}

	return;
}

void Model::Draw()
{
	//unsigned int size = sizeof(Scene);
	//unsigned int offset = 0;
	//
	//D3D11_MAPPED_SUBRESOURCE map;
	//DEMO_APP::devContext->Map(DEMO_APP::SceneCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	//memcpy(map.pData, &DEMO_APP::GetScene(), size);
	//DEMO_APP::devContext->Unmap(DEMO_APP::SceneCbuffer, 0);
	//DEMO_APP::devContext->VSSetConstantBuffers(1, 1, &DEMO_APP::GetSceneCBuffer());
	//
	//size = sizeof(DEMO_APP::SIMPLE_VERTEX);
	//DEMO_APP::devContext->IASetVertexBuffers(0, 1, &Vertbuffer, &size, &offset);
	//
	//DEMO_APP::devContext->IASetIndexBuffer(Indexbuffer, DXGI_FORMAT_R32_UINT, offset);
	//DEMO_APP::devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//DEMO_APP::devContext->DrawIndexed(numIndices, 0, 0);
}