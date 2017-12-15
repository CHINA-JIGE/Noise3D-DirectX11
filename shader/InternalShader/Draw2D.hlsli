/*******************************************************

	Module: Shader 2D(header)
	Author: Jige

	2D Shader Main Functions
	
*****************************************************/

class RenderTech_DrawSolid2D
{
public:

	static VS_OUTPUT_SIMPLE VS_Solid2D(VS_INPUT_SIMPLE input);

	static PS_OUTPUT_SIMPLE PS_Solid2D(VS_OUTPUT_SIMPLE input);
};


technique11 DrawSolid2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, RenderTech_DrawSolid2D::VS_Solid2D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, RenderTech_DrawSolid2D::PS_Solid2D()));
	}
}

/*technique11 DrawTextured2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Textured2D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Textured2D()));
	}
}

technique11 DrawText2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawText2D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawText2D()));
		//SetDepthStencilState(LessEqualDSS, 0);
	}
}*/