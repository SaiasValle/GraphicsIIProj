TextureCube CubeTexture : register(t0);
SamplerState Sample : register(s0);

struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float3 uvw : UVW;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	return CubeTexture.Sample(Sample, input.uvw);
}