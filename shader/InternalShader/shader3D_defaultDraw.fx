
/**********************************************************************

	File:3D Function.fx
	Author: Jige
	
	Child Function used by 3d Shaders

**************************************************************************/



Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSpecularMap;
TextureCube gCubeMap;





/**********************************************************************
					SHADING FUNCTION
**************************************************************************/
//function used by all light-computing functions
void	ComputeOutput_Amb_Diff_Spec(float diffuseCosFactor, float lightSpecIntensity, float matSpecSmoothLevel, float Attenuation,
										float3 matAmbientColor3	, float3 lightAmbientColor3,
										float3 matDiffuseColor3	, float3 lightDiffuseColor3, 
										float3 matSpecColor3	, float3 lightSpecColor3, 
										float3 Vec_toCam, float3 UnitLightVec, float3 Normal,
										out float4 outAmbient,out float4 outDiffuse,out float4 outSpecular)
{

	outAmbient = float4(0,0,0,1.0f);
	outDiffuse = float4(0,0,0,1.0f);
	outSpecular = float4(0,0,0,1.0f);
	
	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient = float4(matAmbientColor3*lightAmbientColor3*matDiffuseColor3, 1.0f);

	[flatten]
	
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if(diffuseCosFactor > 0.0f)
	{

		outDiffuse = Attenuation* diffuseCosFactor* float4(matDiffuseColor3 * lightDiffuseColor3,1.0f);
		
		//now specular	----reflect () : input an incoming light,and output an outgoing light , the axis being reflected about is a bisecting vector of NORMAL and LIGHTVEC
		float3 tmpV = reflect(UnitLightVec,normalize(Normal));
		
		//remember to normalize vectors to be "dotted"
		Vec_toCam = normalize(Vec_toCam);
		
		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = lightSpecIntensity * pow(max(dot(tmpV,Vec_toCam),0.0f),matSpecSmoothLevel);
		
		//final SpecularColor
		outSpecular = Attenuation * SpecFactor * float4(matSpecColor3 *lightSpecColor3,1.0f);
	}

}




void	SampleFromTexture(float2 TexCoord,out float3 outDiffColor3,out float3 outNormalTBN,out float3 outSpecColor3)
{
	//let's see if diffuse map is valid, and determine whether to use diffuse map
	outDiffColor3 = float3(0, 0, 0);
	outSpecColor3 = float3(0,0,0);
	outNormalTBN = float3(0,0,0);
	
	//------------Diffuse Map------------
	[flatten]
	float3 diffSampleColor = gDiffuseMap.Sample(samplerDefault, TexCoord).xyz;
	if (gIsDiffuseMapValid)
	{
		outDiffColor3 = diffSampleColor;
	}
	else
	{
		//invalid diffuse map, we should use pure color of basic material
		outDiffColor3 = gMaterial.mDiffuseColor;
	}
	

	//-------NORMAL MAP-------
	float3 normSampleColor = gNormalMap.Sample(samplerDefault, TexCoord).xyz;
	if(gIsNormalMapValid)
	{
		//retrieve normals from normal map
		outNormalTBN = normSampleColor;
		//map to [-1,1]
		outNormalTBN = 2 * outNormalTBN - 1;
		//scale bump mapping
		outNormalTBN = normalize(outNormalTBN + saturate(1.0f-gMaterial.mNormalMapBumpIntensity) *(float3(0,1.0f,0)-outNormalTBN));
	}
	else
	{
		outNormalTBN = float3(0,1.0f,0);
	}
	

	//----------SPECULAR MAP--------------
	float3 specSampleColor = gDiffuseMap.Sample(samplerDefault, TexCoord).xyz;
	if(gIsSpecularMapValid)
	{
		outSpecColor3 = specSampleColor;
	}
	else
	{
	//invalid diffuse map, we should use pure color of basic material
		outSpecColor3 = gMaterial.mSpecularColor;
	}
	
}


void	SampleFromEnvironmentMap(float3 VecToCamW, float3 NormalW, out float4 outEnvMapColor4)
{
	outEnvMapColor4 = float4(0,0,0,0);

	float4 sampleColor = float4(gCubeMap.Sample(samplerDefault, reflect(-VecToCamW, NormalW)).xyz, saturate(gMaterial.mEnvironmentMapTransparency));


	if(gIsEnvironmentMapVaild)
	{
		//alpha : user-set transparency , used to blend
		outEnvMapColor4 = sampleColor;
	}
	else
	{
		outEnvMapColor4 = float4(0,0,0,0);
	}
}




//convert XYZ to TBN
void	TransformCoord_XYZ_TBN(float3 inVectorXYZ,float3 TangentW,float3 NormalW,out float3 outVectorTBN)
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
	TangentW = normalize(TangentW - dot(NormalW,TangentW)*NormalW);//orthogonalization , subtract  tangent's projection on Normal Vector
	float3 BinormalW = normalize(cross(NormalW,TangentW));
	
	//x - Binormal , Y-Normal ,Z - tangent
	/*float4x4 transformMatrix;
	transformMatrix[0] = float4(BinormalW.x,NormalW.x,TangentW.x,0);
	transformMatrix[1] = float4(BinormalW.y,NormalW.y,TangentW.y,0);
	transformMatrix[2] = float4(BinormalW.z,NormalW.z,TangentW.z,0);
	transformMatrix[3] = float4(0,0,0,1.0f);*/
	float4x4 transformMatrix;
	transformMatrix[0] = float4(TangentW.x,NormalW.x,BinormalW.x,0);
	transformMatrix[1] = float4(TangentW.y,NormalW.y,BinormalW.y,0);
	transformMatrix[2] = float4(TangentW.z,NormalW.z,BinormalW.z,0);
	transformMatrix[3] = float4(0, 0, 0, 1.0f);

	outVectorTBN = mul(float4(inVectorXYZ,1.0f),transformMatrix).xyz;
}


//convert TBN to XYZ
void	TransformCoord_TBN_XYZ(float3 inVectorTBN, float3 TangentW, float3 NormalW,out float3 outVectorXYZ)
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
	TangentW = normalize(TangentW - dot(NormalW,TangentW)*NormalW);//orthogonalization , subtract  tangent's projection on Normal Vector
	float3 BinormalW = normalize(cross(NormalW,TangentW));
	
	//x - Binormal , Y-Normal ,Z - tangent
	/*float4x4 transformMatrix;
	transformMatrix[0] = float4(BinormalW.xyz,0);
	transformMatrix[1] = float4(NormalW.xyz,0);
	transformMatrix[2] = float4(TangentW.xyz,0);
	transformMatrix[3] = float4(0,0,0,1.0f);*/
	float4x4 transformMatrix;
	transformMatrix[0] = float4(TangentW.xyz,0);
	transformMatrix[1] = float4(NormalW.xyz,0);
	transformMatrix[2] = float4(BinormalW.xyz,0);
	transformMatrix[3] = float4(0,0,0,1.0f);

	//outVectorXYZ = mul(float4(inVectorTBN,1.0f),transformMatrix).xyz;
	outVectorXYZ = mul(float4(inVectorTBN, 1.0f),transformMatrix).xyz;
}



//color computation for dir light
void	ComputeDirLightColor(N_DirectionalLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 tangentW,
							out float4 outColor4)
{
	outColor4 = float4(0,0,0,1.0f);
	float4 outAmbient4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outDiffuse4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outSpec4		= float4(0.0f,0.0f,0.0f,0.0f);


	//direction of the incoming light
	float3	LightVec = Light.mDirection;
	float3 	Unit_LightVec =  normalize(LightVec);
	

	
	//sample colors / normal  from textures
	float3 diffuseColor3 = float3(0, 0, 0);
	float3 normalTBN = float3(0,1.0f,0);
	float3 specularColor3 = float3(0,0,0);
	float4 envMapColor4 = float4(0,0,0,0);
	float3 lightVecTBN = float3(0,0,0);
	float3 Vec_toCamTBN = float3(0,0,0);
	float3 deviatedNormalW = float3(0,0,0);
	
	SampleFromTexture(TexCoord,diffuseColor3,normalTBN,specularColor3);
	TransformCoord_TBN_XYZ(normalTBN,tangentW,NormalW,deviatedNormalW);//to make the reflection being affected by normal deviation
	SampleFromEnvironmentMap(Vec_toCam,deviatedNormalW,envMapColor4);//we must get the alpha to blend
	TransformCoord_XYZ_TBN(Vec_toCam,tangentW,NormalW,Vec_toCamTBN);
	TransformCoord_XYZ_TBN(Unit_LightVec,tangentW,NormalW,lightVecTBN);
	
	float diffuseCosFactor = Light.mDiffuseIntensity *dot(-lightVecTBN,normalTBN);
	
	
	//transform XYZ coord to TBN
	
	
	ComputeOutput_Amb_Diff_Spec(diffuseCosFactor,Light.mSpecularIntensity,gMaterial.mSpecularSmoothLevel,1.0f,
									gMaterial.mAmbientColor	,Light.mAmbientColor,
									diffuseColor3						,Light.mDiffuseColor,
									specularColor3					,Light.mSpecularColor,
									Vec_toCamTBN,lightVecTBN,normalTBN,
									outAmbient4,outDiffuse4,outSpec4);
	

	outColor4 = float4(lerp(outAmbient4.xyz + outDiffuse4.xyz,envMapColor4.xyz,envMapColor4.w),1.0f) + outSpec4;
};





void	ComputePointLightColor(N_PointLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 thisPoint,float3 tangentW,
							out float4 outColor4)
{
	outColor4 = float4(0,0,0,1.0f);
	float4 outAmbient4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outDiffuse4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outSpec4		= float4(0.0f,0.0f,0.0f,0.0f);


	// direction of the incoming light
	float3	LightVec = thisPoint - Light.mPosition;
	float3	Unit_LightVec = normalize(LightVec);

	//.............
	float disFromLight = length(LightVec);
	float Attenuation = 1 / (Light.mAttenuationFactor*disFromLight*disFromLight + 1);

	
	//to check if this point is out of lighting range :: DISTANCE /ANGLE RANGE
	if(disFromLight > Light.mLightingRange)
	{
		outColor4 = float4(0,0,0,1.0f);
		return;
	}

	
	//sample colors / normal  from textures
	float3 diffuseColor3 = float3(0, 0, 0);
	float3 normalTBN = float3(0,1.0f,0);
	float3 specularColor3 = float3(0,0,0);
	float4 envMapColor4 = float4(0,0,0,0);
	float3 lightVecTBN = float3(0,0,0);
	float3 Vec_toCamTBN = float3(0,0,0);
	float3 deviatedNormalW = float3(0,0,0);//be deviated by normal map
	
	SampleFromTexture(TexCoord,diffuseColor3,normalTBN,specularColor3);
	TransformCoord_TBN_XYZ(normalTBN,tangentW,NormalW,deviatedNormalW);//to make the reflection being affected by normal deviation
	SampleFromEnvironmentMap(Vec_toCam,deviatedNormalW,envMapColor4);//we must get the alpha to blend
	TransformCoord_XYZ_TBN(Vec_toCam,tangentW,NormalW,Vec_toCamTBN);
	TransformCoord_XYZ_TBN(Unit_LightVec,tangentW,NormalW,lightVecTBN);

	
	float diffuseCosFactor =Light.mDiffuseIntensity * dot(-Unit_LightVec,NormalW);
	
	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient4 = float4(gMaterial.mAmbientColor*Light.mAmbientColor*diffuseColor3, 1.0f);
	
	
	ComputeOutput_Amb_Diff_Spec(diffuseCosFactor,Light.mSpecularIntensity,gMaterial.mSpecularSmoothLevel,Attenuation,
									gMaterial.mAmbientColor	,Light.mAmbientColor,
									diffuseColor3			,Light.mDiffuseColor,
									specularColor3			,Light.mSpecularColor,
									Vec_toCamTBN,lightVecTBN,normalTBN,
									outAmbient4,outDiffuse4,outSpec4);

	//we must blend the environment map reflection color first, then add specular color
	outColor4 = float4(lerp(outAmbient4.xyz + outDiffuse4.xyz,envMapColor4.xyz,envMapColor4.w),1.0f) + outSpec4;
};





void	ComputeSpotLightColor(N_SpotLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 thisPoint,float3 tangentW,
							out float4 outColor4)
{
	outColor4 = float4(0,0,0,1.0f);
	float4 outAmbient4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outDiffuse4		= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outSpec4			= float4(0.0f,0.0f,0.0f,0.0f);

	// direction of the incoming light
	float3	LightVec = thisPoint - Light.mPosition;
	float3	Unit_LightVec = normalize(LightVec);

	//.............
	float disFromLight = length(LightVec);
	float Attenuation = 1 / (Light.mAttenuationFactor*disFromLight*disFromLight + 1);
	
	//the cos(theta) of the angle between direction and current light ray
	//this COSINE is used to jugde if the light ray is in the cone
	float Cos_Theta = dot(Unit_LightVec,normalize(Light.mLitAt - Light.mPosition));
	
	//to check if this point is out of lighting range
	if((disFromLight > Light.mLightingRange)||(Cos_Theta < cos(Light.mLightingAngle/2.0f)))
	{
		outColor4 = float4(0,0,0,1.0f);
		return;
	}
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Should I add a fade-out region for the edge of spot-light??? I just directly & violently set it to black???

	
	//sample colors / normal  from textures
	float3 diffuseColor3 = float3(0,0,0);
	float3 normalTBN = float3(0,1.0f,0);
	float3 specularColor3 = float3(0,0,0);
	float3 lightVecTBN = float3(0,0,0);
	float3 Vec_toCamTBN = float3(0,0,0);
	float4 envMapColor4 = float4(0,0,0,0);
	float3 deviatedNormalW = float3(0,0,0);//be deviated by normal map
	
	SampleFromTexture(TexCoord,diffuseColor3,normalTBN,specularColor3);
	TransformCoord_TBN_XYZ(normalTBN,tangentW,NormalW,deviatedNormalW);//to make the reflection being affected by normal deviation
	SampleFromEnvironmentMap(Vec_toCam,deviatedNormalW,envMapColor4);//we must get the alpha to blend
	TransformCoord_XYZ_TBN(Vec_toCam,tangentW,NormalW,Vec_toCamTBN);
	TransformCoord_XYZ_TBN(Unit_LightVec,tangentW,NormalW,lightVecTBN);
	
	//Diffuse factor ::  Cos(theta)
	float diffuseCosFactor = Light.mDiffuseIntensity *dot(-Unit_LightVec,NormalW);
	
	
	ComputeOutput_Amb_Diff_Spec(diffuseCosFactor,Light.mSpecularIntensity,gMaterial.mSpecularSmoothLevel,Attenuation,
									gMaterial.mAmbientColor	,Light.mAmbientColor,
									diffuseColor3			,Light.mDiffuseColor,
									specularColor3,			Light.mSpecularColor,
									Vec_toCamTBN,Unit_LightVec,normalTBN,
									outAmbient4,outDiffuse4,outSpec4);

									
	outColor4 = float4(lerp(outAmbient4.xyz + outDiffuse4.xyz,envMapColor4.xyz,envMapColor4.w),1.0f) + outSpec4;
};