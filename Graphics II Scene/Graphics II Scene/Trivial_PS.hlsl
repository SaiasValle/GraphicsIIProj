
struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	return input.colorOut;
}