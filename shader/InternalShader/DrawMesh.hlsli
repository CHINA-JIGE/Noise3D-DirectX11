/******************************************

	Module: Draw Mesh(header)
	Author: Jige
	
	draw mesh technique

******************************************/

class RenderProcess_DrawMesh
{
public:

	RenderProcess_DrawMesh();

private:
	bool mEnableVertexLighting;
	bool mEnableDiffuseMap;
	bool mEnableNormalMap;
	bool mEnableSpecularMap;
	bool mEnableEnvMap;
};



/*technique11 DrawMesh
{
pass Pass0
{
SetVertexShader(CompileShader(vs_5_0,VS_DrawMesh()));
SetGeometryShader(NULL);
SetPixelShader(CompileShader(ps_5_0,PS_DrawMesh()));
}
}*/

technique11 DrawMesh
{
	pass Pass000
	{
		//gouraud-shading ||| diffuse map ||| normal map ||| specular map ||| env map
		SetVertexShader(CompileShader(vs_5_0, VS_DrawMesh(false, false, false, false, false)));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawMesh(false, false, false, false, false)));
	}
}
