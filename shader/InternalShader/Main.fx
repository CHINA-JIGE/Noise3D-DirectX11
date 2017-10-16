/**********************************************************************

	File: Main.fx
	Author: Jige
	
	Main FX Compilation entry

**************************************************************************/
#include "GlobalDef.fx"
#include "shader3D_defaultDraw.fx"
#include "shader3D.fx"
#include "shader2D.fx"
#include "picking.fx"

//---------------------------------------------------------------------------------------

technique11 DefaultDraw
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0,VS_DefaultDraw()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0,PS_DefaultDraw()));
	}
}

technique11 DrawSolid3D
{
	pass EmptyTextureSky
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid3D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Solid3D()));
	}
}

technique11 DrawSolid2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid2D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Solid2D()));
	}
}

technique11 DrawTextured2D
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
}

technique11 DrawSky
{
	//one of the pass will be chosen to draw sky
	pass EmptySky
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawSky()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSky(false, false)));//skyBox,skyDome
	}
	pass DrawSkyBox
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawSky()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSky(true,false)));
	}
	pass DrawSkyDome
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawSky()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSky(false, true)));
	}
}

technique11 PickingIntersection
{
	pass Pass0
	{
	  SetVertexShader(CompileShader(vs_5_0,VS_Picking()));
	  //SetGeometryShader(ConstructGSWithSO(CompileShader(gs_5_0,GS_STREAMOUT()),"POSITION.xyz;NORMAL.xyz;TEXCOORD.xy") );
	  //what is the use of this string....define the format of SO??No seems that it's the format of VS output
	  SetGeometryShader(ConstructGSWithSO(CompileShader(gs_5_0, GS_Picking()), "POSITION0.xyz"));
	  SetPixelShader(NULL);

	}
}