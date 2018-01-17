/******************************************

	Module: Draw Mesh(implementation)
	Author: Jige
	
	draw mesh technique

******************************************/
#include "DrawMesh_Common.fx"
#include "DrawMesh_Gouraud.fx"
#include "DrawMesh_Phong.fx"

VS_OUTPUT_DRAW_MESH_PHONG VS_DrawMeshWithPixelLighting(VS_INPUT_DRAW_MESH input)
{
	//initialize
	VS_OUTPUT_DRAW_MESH_PHONG output = (VS_OUTPUT_DRAW_MESH_PHONG)0;
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

PS_OUTPUT_DRAW_MESH PS_DrawMeshWithPixelLighting(VS_OUTPUT_DRAW_MESH_PHONG input,
	uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap)
{
	//the output
	PS_OUTPUT_DRAW_MESH psOutput=(PS_OUTPUT_DRAW_MESH)0;

	//if the lighting system and material are invalid ,then use vertex color
	if (!gIsLightingEnabled_Dynamic)
	{
		psOutput.color = input.color;
		return psOutput;//vertex color
	}

	//interpolation can  'unnormalized' the unit  vector
	input.normalW = normalize(input.normalW);
	input.tangentW = normalize(input.tangentW);

	//invoke the process of lighting/mapping/shading
	float4 	finalColor4 = float4(0, 0, 0, 1.0f);
	RenderProcess_Phong renderProc;
	renderProc.InitVectors(input.normalW,input.texcoord, input.posW, input.tangentW);
	renderProc.ComputeFinalColor(bDiffMap, bNormalMap, bSpecMap, bEnvMap, finalColor4);

	psOutput.color = finalColor4;
	return psOutput;
}




VS_OUTPUT_DRAW_MESH_GOURAUD VS_DrawMeshWithVertexLighting(VS_INPUT_DRAW_MESH input)
{
	//initialize
	VS_OUTPUT_DRAW_MESH_GOURAUD output = (VS_OUTPUT_DRAW_MESH_GOURAUD)0;
	//the W transformation
	output.posW = mul(float4(input.posL, 1.0f), gWorldMatrix).xyz;
	//the VP transformation
	output.posH = mul(mul(float4(output.posW, 1.0f), gViewMatrix), gProjMatrix);
	//we need an normal vector in W space
	output.normalW = mul(float4(input.normalL, 1.0f), gWorldInvTransposeMatrix).xyz;
	//texture coordinate
	output.texcoord = input.texcoord;

	//vertex lighting
	float4 	ambientLightingColor = float4(0, 0, 0, 1.0f);
	float4 	diffuseLightingColor = float4(0, 0, 0, 1.0f);
	float4  specularLightingColor = float4(0, 0, 0, 1.0f);
	RenderProcess_Gouraud renderProc;
	renderProc.ComputeLightingColorForVS(output.normalW, output.posW, ambientLightingColor, diffuseLightingColor, specularLightingColor);
	
	//because in Pixel shader, diffuse color need to be mul by albedo/diffuseMapColor
	//hence amb/diff/spec are passed to PS seperately
	output.ambient = ambientLightingColor;
	output.diffuse = diffuseLightingColor;
	output.specular = specularLightingColor;

	return output;
}

PS_OUTPUT_DRAW_MESH PS_DrawMeshWithVertexLighting(VS_OUTPUT_DRAW_MESH_GOURAUD input, uniform bool bDiffMap)
{
	float4 	outColor4 = float4(0, 0, 0, 1.0f);
	RenderProcess_Gouraud renderProc;
	renderProc.ComputeFinalColorForPS(
		input.normalW,
		input.posW,
		input.texcoord,
		input.ambient,
		input.diffuse,
		input.specular,
		bDiffMap,
		outColor4);


	PS_OUTPUT_DRAW_MESH psOutput = (PS_OUTPUT_DRAW_MESH)0;
	psOutput.color = outColor4;
	return psOutput;
}
