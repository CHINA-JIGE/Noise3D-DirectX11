/*******************************************************

	Module: Shader 2D(header)
	Author: Jige

	2D Shader Main Functions
	
*****************************************************/

//----------------SOLID 2D-----------------------
VS_OUTPUT_SIMPLE VS_Solid2D(VS_INPUT_SIMPLE input);

PS_OUTPUT_SIMPLE PS_Solid2D(VS_OUTPUT_SIMPLE input);

technique11 DrawSolid2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid2D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Solid2D()));
	}
}

//--------------------TEXTURED 2D------------------------
VS_OUTPUT_SIMPLE VS_Textured2D(VS_INPUT_SIMPLE input);

PS_OUTPUT_SIMPLE PS_Textured2D(VS_OUTPUT_SIMPLE input);

technique11 DrawTextured2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Textured2D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Textured2D()));
	}
}

//-----------------------TEXT 2D---------------------
VS_OUTPUT_SIMPLE VS_DrawText2D(VS_INPUT_SIMPLE input);

PS_OUTPUT_SIMPLE PS_DrawText2D(VS_OUTPUT_SIMPLE input);

technique11 DrawText2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawText2D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawText2D()));
	}
}