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

GS_INPUT_MINIZED VS_Picking(VS_INPUT_DRAW_MESH input);

[maxvertexcount(3)]
void GS_Picking(triangle GS_INPUT_MINIZED  inputGeometry[3], inout PointStream<GS_OUTPUT_MINIZED> pointStream);

bool IntersectTriangle(float3 origin, float3 dir, float3 v0, float3 v1, float3 v2, out float t, out float u, out float v);

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