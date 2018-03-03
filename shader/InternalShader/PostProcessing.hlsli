/******************************************

		Module:  Post processing
		Author: Jige
	
		each pass apply a post-process effect
		to input, passes can be combined arbitrarily

******************************************/

//*************************Shader Entry Definition***********************
VS_OUTPUT_SIMPLE VS_PostProcess_FullScreenQuad(VS_INPUT_SIMPLE input);

VS_OUTPUT_QWERTY VS_PostProcess_QwertyDistortion(VS_INPUT_DRAW_MESH input);

PS_OUTPUT_SIMPLE PS_PostProcess_GreyScale(VS_OUTPUT_SIMPLE input);

PS_OUTPUT_SIMPLE PS_PostProcess_QwertyDistortion(VS_OUTPUT_QWERTY input);


//*****************************Technique Definition****************************
technique11 PostProcessing
{
	pass GreyScale
	{
		SetVertexShader(CompileShader(vs_5_0, VS_PostProcess_FullScreenQuad()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_PostProcess_GreyScale()));
	}

	pass QwertyDistortion
	{
		SetVertexShader(CompileShader(vs_5_0, VS_PostProcess_QwertyDistortion()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_PostProcess_QwertyDistortion()));
	}
}
