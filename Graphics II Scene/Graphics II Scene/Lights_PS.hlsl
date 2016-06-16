Texture2D Texture : register(t0);
SamplerState Sample : register(s0);

cbuffer pointLight : register(b1)
{
	float4 color;
	float4 position;
};
cbuffer directionLight : register(b2)
{
	float4 direction;
	float4 Dcolor;
};
cbuffer spotLight : register(b3)
{
	float4 Sposition;
	float4 Sdirection;
	float4 Scolor;
	float4 spot;
	float4 camera;
};

struct INPUT_PIXEL
{
	float4 coordinate : POSITION;
	float4 color : COLOR;
	float2 uv : UV;
	float3 normal : NORMAL;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	return Texture.Sample(Sample, input.uv);
}