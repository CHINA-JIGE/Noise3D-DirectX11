/**********************************************************

		Module:  Post processing
		Author: Jige
	
		Qwerty 3D - perspective correct rendering

**********************************************************/




VS_OUTPUT_SIMPLE VS_PostProcess_QwertyDistortion(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE vsOutput = (VS_OUTPUT_SIMPLE)0;
	vsOutput.posH = float4(input.posL, 1.0f);
	vsOutput.color = input.color;
	vsOutput.texcoord = input.texcoord;
	return vsOutput;
}

PS_OUTPUT_SIMPLE PS_PostProcess_QwertyDistortion(VS_OUTPUT_SIMPLE input)
{
	PS_OUTPUT_SIMPLE psOutput = (PS_OUTPUT_SIMPLE)0;
	float4 currentPixel = gPreviousRenderTarget.Sample(samplerDraw2D, input.texcoord);
	psOutput.color = currentPixel;
	return psOutput;
}
