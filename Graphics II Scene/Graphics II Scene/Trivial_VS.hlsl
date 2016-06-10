#pragma pack_matrix(row_major)
struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float4 color : COLOR;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
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
		
	// TODO : PART 4 STEP 4
	/*sendToRasterizer.projectedCoordinate.xy += constantOffset;*/
	
	// TODO : PART 3 STEP 7
	sendToRasterizer.colorOut = fromVertexBuffer.color;
	// END PART 3

	return sendToRasterizer;
}