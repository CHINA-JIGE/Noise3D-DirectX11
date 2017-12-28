/******************************************

	Module: Draw Mesh(implementation)
	Author: Jige
	
	draw mesh technique

******************************************/

#include "DrawMesh_Common.fx"
#include "DrawMesh_PixelLighting.fx"
#include "DrawMesh_VertexLighting.fx"

VS_OUTPUT_DRAW_MESH VS_DrawMeshWithPixelLighting(VS_INPUT_DRAW_MESH input)
{
	//initialize
	VS_OUTPUT_DRAW_MESH output = (VS_OUTPUT_DRAW_MESH)0;
	//the W transformation
	output.posW = mul(float4(input.posL, 1.0f), gWorldMatrix).xyz;
	//the VP transformation
	output.posH = mul(mul(float4(output.posW, 1.0f), gViewMatrix), gProjMatrix);
	//output the vertex color , this parameter will be used if the lighting system is off
	output.color = input.color;
	//we need an normal vector in W space
	output.normalW = mul(float4(input.normalL, 1.0f), gWorldInvTransposeMatrix).xyz;
	//transform tangent to help implement XYZ to TBN
	output.tangentW = mul(float4(input.tangentL, 0.0f), gWorldMatrix).xyz;
	//texture coordinate
	output.texcoord = input.texcoord;

	return output;
}

PS_OUTPUT_DRAW_MESH PS_DrawMeshWithPixelLighting(VS_OUTPUT_DRAW_MESH input,
	uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap)
{
	//the output
	float4 	finalColor4 = float4(0, 0, 0, 0);
	float4	tmpColor4 = float4(0, 0, 0, 0);

	//if the lighting system and material are invalid ,then use vertex color
	if ((!gIsLightingEnabled_Dynamic) && (!gIsLightingEnabled_Static))
	{
		return input.color;//vertex color
	}

	//interpolation can  'unnormalized' the unit  vector
	input.normalW = normalize(input.normalW);
	input.tangentW = normalize(input.tangentW);

	//vector ---- this point to Camera
	float3 Vec_ToCam = gCamPos - input.posW;


	//compute fog effect
	//skip pixel if it is totally fogged
	float Dist_CurrPointToCam = length(Vec_ToCam);
	if (gFogEnabled == 1 && Dist_CurrPointToCam>gFogFar)
	{
		return float4(gFogColor3, 1.0f);
	}

	在这里初始化那个RenderProcess类

	//compute DYNAMIC LIGHT
	int i = 0;
	if (gIsLightingEnabled_Dynamic)
	{
		for (i = 0; i<gDirectionalLightCount_Dynamic; i++)
		{
			都换成ComputeLightColor 
			ComputeDirLightColor(gDirectionalLight_Dynamic[i], input.normalW, input.texcoord, Vec_ToCam, input.tangentW, tmpColor4);
			finalColor4 += tmpColor4;
		}
		for (i = 0; i<gPointLightCount_Dynamic; i++)
		{
			ComputePointLightColor(gPointLight_Dynamic[i], input.normalW, input.texcoord, Vec_ToCam, input.posW, input.tangentW, tmpColor4);
			finalColor4 += tmpColor4;
		}
		for (i = 0; i<gSpotLightCount_Dynamic; i++)
		{

			ComputeSpotLightColor(gSpotLight_Dynamic[i], input.normalW, input.texcoord, Vec_ToCam, input.posW, input.tangentW, tmpColor4);
			finalColor4 += tmpColor4;
		}
	}

	//compute STATIC LIGHT
	if (gIsLightingEnabled_Static)
	{
		for (i = 0; i<gDirectionalLightCount_Static; i++)
		{
			ComputeDirLightColor(gDirectionalLight_Static[i], input.normalW, input.texcoord, Vec_ToCam, input.tangentW, tmpColor4);
			finalColor4 += tmpColor4;
		}
		for (i = 0; i<gPointLightCount_Static; i++)
		{
			ComputePointLightColor(gPointLight_Static[i], input.normalW, input.texcoord, Vec_ToCam, input.posW, input.tangentW, tmpColor4);
			finalColor4 += tmpColor4;
		}
		for (i = 0; i<gSpotLightCount_Static; i++)
		{
			ComputeSpotLightColor(gSpotLight_Static[i], input.normalW, input.texcoord, Vec_ToCam, input.posW, input.tangentW, tmpColor4);
			finalColor4 += tmpColor4;
		}
	}

	//at last compute fog  ( point farther than gFogFar has been skipped
	if (gFogEnabled)
	{
		float fogInterpolationFactor = max(0, (Dist_CurrPointToCam - gFogNear) / (gFogFar - gFogNear));
		finalColor4 = lerp(finalColor4, float4(gFogColor3, 1.0f), fogInterpolationFactor);
	}

	//set transparency component
	finalColor4.w = saturate(gMaterial.mTransparency);

	PS_OUTPUT_DRAW_MESH psOutput;
	psOutput.color = finalColor;
	return psOutput;
}
