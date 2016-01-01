
/**********************************************************************

			File:Input Struct.fx
			input struct used by main.fx by VS/PS/xxx main shader functions

**************************************************************************/

#ifndef INPUT_STRUCT_H
#define INPUT_STRUCT_H


/**********************************************************************
			INPUT   STRUCTURE    DEFINITION
**************************************************************************/
struct VS_INPUT_DEFAULT
{
	float3 posL : POSITION;
	float4 color : COLOR;
	float3 normalL :NORMAL;
	float2 texcoord: TEXCOORD;
	float3 tangentL : TANGENT;
};

struct VS_OUTPUT_DEFAULT
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
	float3 posW: POSITION;
	float3 normalW :NORMAL;
	float2 texcoord:TEXCOORD;
	float3 tangentW :TANGENT;
};

struct VS_INPUT_SIMPLE
{
	float3 posL:POSITION;
	float4 color:COLOR;
	float2 texcoord:TEXCOORD;
};

struct VS_OUTPUT_SIMPLE
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord:TEXCOORD;
};

#endif