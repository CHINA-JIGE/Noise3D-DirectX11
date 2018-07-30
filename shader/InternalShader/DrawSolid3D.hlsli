/******************************************

	Module: Draw Solid 3D(header)
	Author: Jige
	
	draw solid 3D mesh 
	(pure color, simplified vertex)

******************************************/

VS_OUTPUT_SIMPLE VS_Solid3D(VS_INPUT_SIMPLE input);

PS_OUTPUT_SIMPLE PS_Solid3D(VS_OUTPUT_SIMPLE input);

technique11 DrawSolid3D
{
	//pass EmptyTextureSky
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid3D()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Solid3D()));
	}
}

