/// Header file for storing vertex declarations for input and out from vertex shaders

#ifndef VERTEX_LAYOUTS_HLSLI
#define VERTEX_LAYOUTS_HLSLI

////////////////////////////////////////////////////////////
//					Input vertices						  //
////////////////////////////////////////////////////////////
struct VERTEX_POS
{
	float3 position :	POSITION;
};

struct VERTEX_POSCOLOR
{
	float3 position :	POSITION;
	float4 color	:	COLOR;
};

struct VERTEX_POSTEX
{
	float3 position :	POSITION;
	float2 texCoord :	TEXCOORD;
};

struct VERTEX_POSNORMTEX
{
	float3 position :	POSITION;
	float3 normal	:	NORMAL;
	float2 texCoord :	TEXCOORD;
};

////////////////////////////////////////////////////////////
//					Output vertices						  //
////////////////////////////////////////////////////////////
struct TwoDVertexOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

struct TwoDVertexWithPositionOut
{
	float4 position			: SV_POSITION;
	float4 pixelPosition	: TEXCOORD0;
	float2 texCoord			: TEXCOORD1;
};

struct PosColorOut
{
	float4 position			: SV_POSITION;
	float4 color			: COLOR;
};

struct CubeVertOut
{
	float4 position			: SV_POSITION;
	float3 texCoord			: TEXCOORD0;
};

struct VertexWithPositionOut
{
	float4 position			: SV_POSITION;
	float4 pixelPosition	: TEXCOORD0;
};
