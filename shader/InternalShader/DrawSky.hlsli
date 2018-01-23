/************************************************

	Module: Draw Sky(header)
	Author: Jige

	sky box ,sky dome rendering
	
*************************************************/

VS_OUTPUT_DRAW_MESH_PHONG VS_DrawSky(VS_INPUT_SIMPLE input);

PS_OUTPUT_DRAW_MESH PS_DrawSky(VS_OUTPUT_DRAW_MESH_PHONG input, uniform bool enabledSkyBox, uniform bool enabledSkyDome);

//3 passes are funtionally independent
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
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSky(true, false)));
	}
	pass DrawSkyDome
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawSky()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSky(false, true)));
	}
}