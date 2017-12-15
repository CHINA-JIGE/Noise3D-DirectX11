/******************************************

	Module: picking(header)
	Author: Jige
	
	picking ray & triangle intersection
	(GS involved, accurate)

******************************************/

cbuffer cbPicking
{
	/*(x,y,1)can describe a picking ray , and this var is the xy*/
	float2 gPickingRayNormalizedDirXY;
}


technique11 PickingIntersection
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Picking()));
		//SetGeometryShader(ConstructGSWithSO(CompileShader(gs_5_0,GS_STREAMOUT()),"POSITION.xyz;NORMAL.xyz;TEXCOORD.xy") );
		//what is the use of this string....define the format of SO??No seems that it's the format of VS output
		SetGeometryShader(ConstructGSWithSO(CompileShader(gs_5_0, GS_Picking()), "POSITION0.xyz"));
		SetPixelShader(NULL);
	}
}