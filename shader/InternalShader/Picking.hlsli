/******************************************

	Module: picking(header)
	Author: Jige
	
	picking ray & triangle intersection
	(GS involved, accurate)

******************************************/

GS_INPUT_MINIMIZED VS_Picking(VS_INPUT_DRAW_MESH input);

[maxvertexcount(3)]
void GS_Picking(triangle GS_INPUT_MINIMIZED  inputGeometry[3], inout PointStream<GS_OUTPUT_MINIMIZED> pointStream);

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