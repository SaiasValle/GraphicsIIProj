#pragma pack_matrix(row_major)
struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float4 color : COLOR;
	float2 uv : UV;
	float3 normal : NORMAL;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
	float3 uvOut : UV;
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

OUTPUT_VERTEX main( INPUT_VERTEX fromVertexBuffer )
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	float4 position = mul(fromVertexBuffer.coordinate, worldMatrix);
	position = mul(position, viewMatrix);
	position = mul(position, projectionMatrix);
	
	sendToRasterizer.projectedCoordinate.xyzw = position;	
	sendToRasterizer.colorOut = fromVertexBuffer.color;
	sendToRasterizer.uvOut = fromVertexBuffer.coordinate.xyz;

	return sendToRasterizer;
}