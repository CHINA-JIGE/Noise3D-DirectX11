/******************************************

	Module: ray-mesh intersection(header)
	Author: Jige
	
	ray & triangle mesh intersection
	(GS involved, accurate)

******************************************/

cbuffer cbIntersection
{
	/*(x,y,1)can describe a picking ray , and this var is the xy*/
	float2 gPickingRayNormalizedDirXY;
	float3 gIntersectingRayOrigin;
	float3 gIntersectingRayDir;
}

struct GS_INPUT_DEFAULT_MESH
{
	float3 posW : POSITION;
	float3 normalW :NORMAL;
	float2 texcoord: TEXCOORD;
};

struct GS_OUTPUT_DEFAULT_MESH
{
	float3  pos : POSITION0;
	float t : POSITION1;
	float3 normal :NORMAL;
	float2 texcoord: TEXCOORD;
};

//...
bool IntersectTriangle(float3 origin, float3 dir, float3 v0, float3 v1, float3 v2, out float t, out float u, out float v);

//VS
GS_INPUT_DEFAULT_MESH VS_RayMeshIntersection(VS_INPUT_DRAW_MESH input);

//GS
[maxvertexcount(3)]
void GS_RayMeshIntersection(triangle GS_INPUT_DEFAULT_MESH  inputGeometry[3], inout PointStream<GS_OUTPUT_DEFAULT_MESH> pointStream);


technique11 RayMeshIntersection
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_RayMeshIntersection()));
		//SetGeometryShader(ConstructGSWithSO(CompileShader(gs_5_0,GS_STREAMOUT()),"POSITION.xyz;NORMAL.xyz;TEXCOORD.xy") );
		//what is the use of this string....define the format of SO??No seems that it's the format of VS output
		SetGeometryShader(ConstructGSWithSO(CompileShader(gs_5_0, GS_RayMeshIntersection()), "POSITION0.xyz;POSITION1.x;NORMAL.xyz;TEXCOORD.xy"));
		SetPixelShader(NULL);
	}
}

