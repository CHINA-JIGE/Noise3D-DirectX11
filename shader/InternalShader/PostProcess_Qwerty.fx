/**********************************************************

		Module:  Post processing
		Author: Jige
	
		Qwerty 3D - perspective correct rendering

**********************************************************/



//Note that input vertex format is 'Default Vertex', use IMesh as screen descriptor
VS_OUTPUT_QWERTY VS_PostProcess_QwertyDistortion(VS_INPUT_DRAW_MESH input)
{
	VS_OUTPUT_QWERTY vsOutput = (VS_OUTPUT_QWERTY)0;
	vsOutput.posH = float4(input.texcoord.x*2.0f -1.0f, 1.0f - 2.0f * input.texcoord.y , 0.0f, 1.0f);
	vsOutput.texcoord = input.texcoord;
	vsOutput.posV = mul(float4(input.posL,1.0f), gViewMatrix).xyz;//screen descriptor
	return vsOutput;
}

PS_OUTPUT_SIMPLE PS_PostProcess_QwertyDistortion(VS_OUTPUT_QWERTY input)
{
	PS_OUTPUT_SIMPLE psOutput = (PS_OUTPUT_SIMPLE)0;
	float4 projectedPos4 = mul(float4(input.posV, 1.0f), gProjMatrix);
	float2 sampleTexcoord = float2(
		(projectedPos4.x / projectedPos4.w + 1.0f) / 2.0f,
		(1.0f - projectedPos4.y / projectedPos4.w) / 2.0f);
	float4 currentPixel = gPreviousRenderTarget.Sample(samplerDraw2D, sampleTexcoord);
	psOutput.color = currentPixel;
	return psOutput;
}
