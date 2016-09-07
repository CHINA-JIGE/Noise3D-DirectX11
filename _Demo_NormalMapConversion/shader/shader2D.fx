/**********************************************************************

	File:shader2D.fx
	Author: Jige

	2D Shader Main Functions
	
**************************************************************************/

#include "input_struct.fx"

SamplerState sampler2D_ANISOTROPIC
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 2;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

cbuffer cbDrawText2D
{
	float4 	g2D_TextColor;
	float4	g2D_TextGlowColor;
}

Texture2D g2D_DiffuseMap;



//----------------------------------Draw 2D Lines/Points/Triangles----------------------------------
VS_OUTPUT_SIMPLE VS_Solid2D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = float4(input.posL,1.0f);
	output.color = input.color;
	return output;
}

float4 PS_Solid2D(VS_OUTPUT_SIMPLE input) : SV_Target
{
	return input.color;
}


//----------------------------------Draw Textured 2D Triangles----------------------------------
VS_OUTPUT_SIMPLE VS_Textured2D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = float4(input.posL,1.0f);//mul(mul(float4(input.posL, 1.0f),gViewMatrix), gProjMatrix);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

float4 PS_Textured2D(VS_OUTPUT_SIMPLE input) : SV_Target
{
	float4 sampledColor = g2D_DiffuseMap.Sample(sampler2D_ANISOTROPIC, input.texcoord);
	float4 outputColor = float4(sampledColor.xyz,sampledColor.w * input.color.w);
	return outputColor;
}



//----------------------------------Draw Text/String----------------------------------
VS_OUTPUT_SIMPLE VS_DrawText2D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = float4(input.posL,1.0f);//mul(mul(float4(input.posL, 1.0f),gViewMatrix), gProjMatrix);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

float4 PS_DrawText2D(VS_OUTPUT_SIMPLE input) : SV_Target
{
	float4 sampledColor = g2D_DiffuseMap.Sample(sampler2D_ANISOTROPIC, input.texcoord);
	if(sampledColor.w!=0)sampledColor.xyz=g2D_TextColor.xyz;
	float4 outputColor = float4(sampledColor.xyz,sampledColor.w);
	//float4 outputColor = float4(sampledColor.xyz, input.color.w);
	return outputColor;

}