#pragma pack_matrix(row_major)
#ifndef SKY_BOX_VS_HLSL
#define SKY_BOX_VS_HLSL
#include "VertexLayouts.hlsli"

struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float3 uvw : UVW;
	float3 normals :NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL
};

cbuffer ROTATE : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer VIEW : register(b1)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

struct OUTPUT_VERTEX
{
	float4 pos : SV_POSITION;
	float3 textureCoord : UVW;
};

OUTPUT_VERTEX main(INPUT_VERTEX input)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	float4 position = mul(fromVertexBuffer.coordinate, worldMatrix);
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);

	sendToRasterizer.pos.xyzw = position;
	sendToRasterizer.textureCoord = input.coordinate.xyz;

	// sendToRasterizer.colorOut = fromVertexBuffer.color;

	return sendToRasterizer;
}

#endif //SKY_BOX_VS_HLSL