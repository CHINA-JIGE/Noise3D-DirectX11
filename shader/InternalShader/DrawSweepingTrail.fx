/******************************************

	Module: Draw Sweeping Trail(impl)
	Author: Jige
	
	draw Sweeping Trail (textured)

******************************************/
VS_OUTPUT_SIMPLE VS_DrawSweepingTrail(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = mul(mul(float4(input.posL, 1.0f), gViewMatrix), gProjMatrix);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

PS_OUTPUT_SIMPLE PS_DrawSweepingTrail(VS_OUTPUT_SIMPLE input)
{
	PS_OUTPUT_SIMPLE output = (PS_OUTPUT_SIMPLE)0;
	float4 texColor = gColorMap2D.Sample(samplerDefault, input.texcoord);
	output.color =float4( texColor.rgb,input.color.a);
	return output;
}
