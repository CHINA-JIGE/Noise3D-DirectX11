/*******************************************************

	File:shader2D.fx(implemenetation)
	Author: Jige

	2D Shader Main Functions
	
*****************************************************/

//----------------------------------Draw 2D Lines/Points/Triangles----------------------------------
VS_OUTPUT_SIMPLE VS_Solid2D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = float4(input.posL,1.0f);
	output.color = input.color;
	return output;
}

PS_OUTPUT_SIMPLE PS_Solid2D(VS_OUTPUT_SIMPLE input)
{
	PS_OUTPUT_SIMPLE output;
	output.color = input.color;
	return output;
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

PS_OUTPUT_SIMPLE PS_Textured2D(VS_OUTPUT_SIMPLE input)
{
	PS_OUTPUT_SIMPLE output;
	float4 sampledColor = gColorMap2D.Sample(samplerDraw2D, input.texcoord);
	output.color = float4(sampledColor.xyz,sampledColor.w * input.color.w);
	return output;
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

PS_OUTPUT_SIMPLE PS_DrawText2D(VS_OUTPUT_SIMPLE input)
{
	PS_OUTPUT_SIMPLE output;
	float4 sampledColor = gColorMap2D.Sample(samplerDraw2D, input.texcoord);
	if(sampledColor.w!=0)sampledColor.xyz=g2D_TextColor.xyz;
	output.color = float4(sampledColor.xyz,sampledColor.w);
	//float4 outputColor = float4(sampledColor.xyz, input.color.w);
	return output;

}