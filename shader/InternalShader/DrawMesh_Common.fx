/****************************************************

	File: Draw Mesh(implementation)
	Author: Jige
	
	common function that will be utilized by Draw Mesh shaders

****************************************************/

//convert XYZ to TBN
void	TransformCoord_XYZ_TBN(float3 inVectorXYZ, float3 TangentW, float3 NormalW, out float3 outVectorTBN)
{
	/*
	|(N)
	|
	|
	-------------------(T)
	/
	/
	/  (B)

	*/

	NormalW = normalize(NormalW);
	TangentW = normalize(TangentW - dot(NormalW, TangentW)*NormalW);//orthogonalization , subtract  tangent's projection on Normal Vector
	float3 BinormalW = normalize(cross(NormalW, TangentW));

	//x - Binormal , Y-Normal ,Z - tangent
	/*float4x4 transformMatrix;
	transformMatrix[0] = float4(BinormalW.x,NormalW.x,TangentW.x,0);
	transformMatrix[1] = float4(BinormalW.y,NormalW.y,TangentW.y,0);
	transformMatrix[2] = float4(BinormalW.z,NormalW.z,TangentW.z,0);
	transformMatrix[3] = float4(0,0,0,1.0f);*/
	float4x4 transformMatrix;
	transformMatrix[0] = float4(TangentW.x, NormalW.x, BinormalW.x, 0);
	transformMatrix[1] = float4(TangentW.y, NormalW.y, BinormalW.y, 0);
	transformMatrix[2] = float4(TangentW.z, NormalW.z, BinormalW.z, 0);
	transformMatrix[3] = float4(0, 0, 0, 1.0f);

	outVectorTBN = mul(float4(inVectorXYZ, 1.0f), transformMatrix).xyz;
}

//convert TBN to XYZ
void	TransformCoord_TBN_XYZ(float3 inVectorTBN, float3 TangentW, float3 NormalW, out float3 outVectorXYZ)
{
	/*
	|(N)
	|
	|
	-------------------(T)
	/
	/
	/  (B)

	*/

	NormalW = normalize(NormalW);
	TangentW = normalize(TangentW - dot(NormalW, TangentW)*NormalW);//orthogonalization , subtract  tangent's projection on Normal Vector
	float3 BinormalW = normalize(cross(NormalW, TangentW));

	//x - Binormal , Y-Normal ,Z - tangent
	float4x4 transformMatrix;
	transformMatrix[0] = float4(TangentW.xyz, 0);
	transformMatrix[1] = float4(NormalW.xyz, 0);
	transformMatrix[2] = float4(BinormalW.xyz, 0);
	transformMatrix[3] = float4(0, 0, 0, 1.0f);

	//outVectorXYZ = mul(float4(inVectorTBN,1.0f),transformMatrix).xyz;
	outVectorXYZ = mul(float4(inVectorTBN, 1.0f), transformMatrix).xyz;
}

float3	SampleFromNormalMap(float2 TexCoord, bool enableNormalMap)
{
	if (enableNormalMap)
	{
		float3 normSampleColor = gNormalMap.Sample(samplerDefault, TexCoord).xyz;
		//retrieve normals from normal map
		float3 tmpNormalTBN = normSampleColor;
		//R: [0,1] , G[0,1], B: [0.5,1];
		//R & G~[0,1] map to [-1,1]
		tmpNormalTBN = 2 * tmpNormalTBN - 1;
		//scale bump mapping
		tmpNormalTBN = normalize(tmpNormalTBN + saturate(1.0f - gMaterial.mNormalMapBumpIntensity) *(float3(tmpNormalTBN.x, 0, tmpNormalTBN.z)));
		return tmpNormalTBN;
	}
	else
	{
		return float3(0, 1.0f, 0);
	}
}

float3	SampleFromDiffuseMap(float2 TexCoord, bool enableDiffuseMap)
{
	//------------Diffuse Map------------
	if (enableDiffuseMap)
	{
		return  gDiffuseMap.Sample(samplerDefault, TexCoord).xyz;
	}
	else
	{
		//invalid diffuse map, we should use pure color of basic material
		return gMaterial.mDiffuseColor;
	}
}

float3	SampleFromSpecularMap(float2 TexCoord, bool enableSpecMap)
{
	//----------SPECULAR MAP--------------
	if (enableSpecMap)
	{
		return gDiffuseMap.Sample(samplerDefault, TexCoord).xyz;
	}
	else
	{
		//invalid diffuse map, we should use pure color of basic material
		return gMaterial.mSpecularColor;
	}

}

float4	SampleFromEnvironmentMap(float3 VecToCamW, float3 NormalW, bool enableEnvMap)
{

	if (enableEnvMap)
	{
		//alpha : user-set transparency , used to blend
		return float4(gCubeMap.Sample(samplerDefault, reflect(-VecToCamW, NormalW)).xyz, saturate(gMaterial.mEnvironmentMapTransparency));
	}
	else
	{
		return float4(0, 0, 0, 0);
	}
}


