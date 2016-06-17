Texture2D Texture : register(t0);
SamplerState Sample : register(s0);

struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
	float2 uv : UV;
	float3 normal : NORMAL;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	return input.colorOut;//Texture.Sample(Sample, input.uvw);
}