/******************************************

		Module: Draw Mesh(header)
		Author: Jige
	
		draw mesh technique

******************************************/

//use a class to reduce parameter passing of functions
class RenderProcess_PixelLighting
{
	void InitEffectSwitches(uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap);

	void InitVectors(float3 NormalW, float2 TexCoord, float3 Vec_toCam, float3 thisPoint, float3 tangentW);

	//color compute for dynamic/static -- dir/point/spot light
	void ComputeLightColor(int lightTypeID, int lightIndex,out float4 outColor4);

	bool mEnableDiffuseMap;		//input
	bool mEnableNormalMap;		//input
	bool mEnableSpecularMap;	//input
	bool mEnableEnvMap;			//input

	float3 mNormalW;
	float2 mTexcoord;
	float3 mVecToCamW;
	float3 mPosOfCurrentPointW;
	float3 mTangentW;
};

class RenderProcess_VertexLighting
{
	void InitEffectSwitches(uniform bool bDiffMap);

	void InitVectors();

};

void	TransformCoord_XYZ_TBN(float3 inVectorXYZ, float3 TangentW, float3 NormalW, out float3 outVectorTBN);

void	TransformCoord_TBN_XYZ(float3 inVectorTBN, float3 TangentW, float3 NormalW, out float3 outVectorXYZ);

float3	SampleFromNormalMap(float2 TexCoord, bool enableNormalMap);

float3	SampleFromDiffuseMap(float2 TexCoord, bool enableDiffuseMap);

float3	SampleFromSpecularMap(float2 TexCoord, bool enableSpecMap);

float4	SampleFromEnvironmentMap(float3 VecToCamW, float3 NormalW, bool enableEnvMap);



VS_OUTPUT_DRAW_MESH VS_DrawMeshWithPixelLighting(VS_INPUT_DRAW_MESH input);

PS_OUTPUT_DRAW_MESH PS_DrawMeshWithPixelLighting(VS_OUTPUT_DRAW_MESH input,
	uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap);


//*****************************Technique Definition****************************
technique11 DrawMesh
{
	pass Pass000
	{
		//diffuse map ||| normal map ||| specular map ||| env map
		SetVertexShader(CompileShader(vs_5_0, VS_DrawMeshWithPixelLighting()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_DrawMeshWithPixelLighting(false,false,false,false)));
	}
}
