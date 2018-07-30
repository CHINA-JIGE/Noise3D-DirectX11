/******************************************

	Module: Draw Sweeping Trail (header)
	Author: Jige
	
	draw a sweeping trail (corresponding to Noise3D::ISweepingTrail)

******************************************/

VS_OUTPUT_SIMPLE VS_DrawSweepingTrail(VS_INPUT_SIMPLE input);

PS_OUTPUT_SIMPLE PS_DrawSweepingTrail(VS_OUTPUT_SIMPLE input);

technique11 DrawSweepingTrail
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawSweepingTrail()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSweepingTrail()));
	}
}
