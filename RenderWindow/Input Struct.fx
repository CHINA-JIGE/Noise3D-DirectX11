/**********************************************************************

File: Input Struct.fx
Author: Jige

**************************************************************************/


struct VS_INPUT_DEFAULT
{
	float3 posL : POSITION;
	float4 color : COLOR;
	float3 normalL :NORMAL;
	float2 texcoord: TEXCOORD;
};

struct VS_OUTPUT_DEFAULT
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
	float3 posW: POSITION;
	float3 normalW :NORMAL;
	float2 texcoord:TEXCOORD;
};

struct VS_INPUT_SIMPLE
{
	float3 posL:POSITION;
	float4 color:COLOR;
};

struct VS_OUTPUT_SIMPLE
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
};