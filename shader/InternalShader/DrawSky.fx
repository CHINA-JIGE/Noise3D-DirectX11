/************************************************

	Module: Draw Sky(implementation)
	Author: Jige

	sky box ,sky dome rendering
	
*************************************************/

VS_OUTPUT_DRAW_MESH_PHONG VS_DrawSky(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_DRAW_MESH_PHONG output = (VS_OUTPUT_DRAW_MESH_PHONG)0;

	//so the sky will always be in the same relative position with camera  (ww means the triangles lie in  infinitely far from original point)
	//output.posH = float4(mul(float4(mul(input.posL,viewMatrixWithoutTranslation),1.0f), gProjMatrix).xy,1.0f,1.0f);
	output.posH = mul(float4(mul(float4(input.posL, 0.0f), gViewMatrix).xyz, 1.0f), gProjMatrix).xyww;
	output.posW = input.posL;
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

PS_OUTPUT_DRAW_MESH PS_DrawSky(VS_OUTPUT_DRAW_MESH_PHONG input, uniform bool enabledSkyBox, uniform bool enabledSkyDome)
{
	PS_OUTPUT_DRAW_MESH output = (PS_OUTPUT_DRAW_MESH)0;

	//both skybox and skydome are invalid
	if ((!enabledSkyDome) && (!enabledSkyBox))
	{
		output.color = input.color;
		return output;
	}
	else if (enabledSkyDome)
	{
		output.color = gDiffuseMap.Sample(samplerDefault, input.texcoord);
		return output;
	}
	else if (enabledSkyBox)
	{
		//what we used to intersect sky box  is a world-space Vector , but we should map the irregular skybox to a standard normalized box
		output.color = gCubeMap.Sample(samplerDefault, input.posW * float3(1 / gSkyBoxWidth, 1 / gSkyBoxHeight, 1 / gSkyBoxDepth));
		return output;
	}

	return output;//not reachable
}

