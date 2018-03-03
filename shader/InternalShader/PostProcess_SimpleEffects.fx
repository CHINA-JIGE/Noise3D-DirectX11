
/**********************************************

		File: Post Process : simple effects
		Author: Jige
	
	implementation of simple post processing effects:

	1.convert to grey scale

************************************************/

//full-screen quad for initiating post process pixel shaders
VS_OUTPUT_SIMPLE VS_PostProcess_FullScreenQuad(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE vsOutput = (VS_OUTPUT_SIMPLE)0;
	vsOutput.posH = float4(input.posL,1.0f);
	vsOutput.color = input.color;
	vsOutput.texcoord = input.texcoord;
	return vsOutput;
}

PS_OUTPUT_SIMPLE PS_PostProcess_GreyScale(VS_OUTPUT_SIMPLE input)
{
	PS_OUTPUT_SIMPLE psOutput = (PS_OUTPUT_SIMPLE)0;
	float4 currentPixel = gPreviousRenderTarget.Sample(samplerDraw2D, input.texcoord);
	float avgIntensity = dot(currentPixel.xyz, float3(0.3f, 0.59f, 0.1f));
	psOutput.color = float4(avgIntensity, avgIntensity, avgIntensity, 1.0f);
	return psOutput;
}