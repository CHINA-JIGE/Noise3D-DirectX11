/**********************************************************************

	File: Main.fx
	Author: Jige
	
	Main FX Compilation entry

**************************************************************************/

#include "shader3D.fx"
#include "shader2D.fx"

//---------------------------------------------------------------------------------------

technique11 DefaultDraw
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0,VS_DefaultDraw()));
		SetPixelShader(CompileShader(ps_5_0,PS_DefaultDraw()));
	}
}

technique11 DrawSolid3D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid3D()));
		SetPixelShader(CompileShader(ps_5_0, PS_Solid3D()));
	}
}

technique11 DrawSolid2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid2D()));
		SetPixelShader(CompileShader(ps_5_0, PS_Solid2D()));
	}
}

technique11 DrawTextured2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Textured2D()));
		SetPixelShader(CompileShader(ps_5_0, PS_Textured2D()));
	}
}

technique11 DrawText2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawText2D()));
		SetPixelShader(CompileShader(ps_5_0, PS_DrawText2D()));
		//SetDepthStencilState(LessEqualDSS, 0);
	}
}

technique11 DrawSky
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawSky()));
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSky()));
		//SetDepthStencilState(LessEqualDSS, 0);
	}
}