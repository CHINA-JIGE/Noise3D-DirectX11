
/**********************************************

	File: Draw Mesh - per pixel lighting
	Author: Jige
	
	functions invoked by mesh drawing shader
	(every mapping techniques are available)

************************************************/

void RenderProcess_PixelLighting::InitEffectSwitches(uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap)
{
	mEnableDiffuseMap = bDiffMap;
	mEnableNormalMap = bNormalMap;
	mEnableSpecularMap = bSpecMap;
	mEnableEnvMap = bEnvMap;
}

void RenderProcess_PixelLighting::InitVectors(float3 NormalW, float2 TexCoord, float3 Vec_toCam, float3 thisPoint, float3 tangentW)
{
	mNormalW = NormalW;
	mTexcoord = TexCoord;
	mVecToCamW = Vec_toCam;
	mPosOfCurrentPointW = thisPoint;
	mTangentW = tangentW;
}

void RenderProcess_PixelLighting::ComputeLightColor(int lightTypeID, int lightIndex,out float4 outColor4)
{
	outColor4 = float4(0, 0, 0, 1.0f);
	float3	lightVecW = float3(0, 0, 0);
	float3	unitLightVecW = float3(0,0,0);
	float		attenuation = 0.0f;
	float3	lightAmbientColor3 = float3(0,0,0);
	float3	lightDiffuseColor3 = float3(0, 0, 0);
	float3	lightSpecColor3 = float3(0, 0, 0);
	float		lightSpecIntensity = 0.0f;

	//----------compute LightVec/Attenuation------------
    bool bReturn = RenderProcess_PixelLighting::mFunction_ComputeLightingVariables(
		lightTypeID, lightIndex,  lightVecW, unitLightVecW,  attenuation, 
        lightAmbientColor3, lightDiffuseColor3, lightSpecColor3,lightSpecIntensity);

	if (bReturn)return;

	//----------Sample from maps----------
	float3 diffuseColor3 = float3(0, 0, 0);
	float3 normalTBN = float3(0, 1.0f, 0);
	float3 specularColor3 = float3(0, 0, 0);
	float4 envMapColor4 = float4(0, 0, 0, 0);
	float3 deviatedNormalW = float3(0, 0, 0);
	float3 lightVecTBN = float3(0, 0, 0);
	float3 Vec_toCamTBN = float3(0, 0, 0);
	float   diffuseCosFactor = 0.0f;

	diffuseColor3	=	SampleFromDiffuseMap(mTexcoord, mEnableDiffuseMap);
	normalTBN		=	SampleFromNormalMap(mTexcoord, mEnableNormalMap);
	specularColor3 = SampleFromSpecularMap(mTexcoord, mEnableSpecularMap);
	TransformCoord_TBN_XYZ(normalTBN, mTangentW, mNormalW, deviatedNormalW);
	envMapColor4	= SampleFromEnvironmentMap(mVecToCamW, deviatedNormalW, mEnableEnvMap);//we must get the alpha to blend
	TransformCoord_XYZ_TBN(mVecToCamW, mTangentW, mNormalW, Vec_toCamTBN);
	TransformCoord_XYZ_TBN(unitLightVecW, mTangentW, mNormalW, lightVecTBN);

    //diffuse cos factor
	diffuseCosFactor = mFunction_ComputeDiffuseCosineFactor(lightTypeID, lightIndex, unitLightVecW, mNormalW);

    float4 outAmbient4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 outDiffuse4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 outSpecular4 = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient4 = float4(gMaterial.mAmbientColor*lightAmbientColor3, 1.0f);

	[flatten]
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if (diffuseCosFactor > 0.0f)
	{
		outDiffuse4 = attenuation* diffuseCosFactor* float4(diffuseColor3 * lightDiffuseColor3, 1.0f);

		//now specular	----reflect () : input an incoming light,and output an outgoing light , the axis being reflected about is a bisecting vector of NORMAL and LIGHTVEC
		float3 tmpV = reflect(unitLightVecW, normalize(mNormalW));

		//remember to normalize vectors to be "dotted"
		float3 unitVecToCam = normalize(mVecToCamW);

		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = lightSpecIntensity * pow(max(dot(tmpV, unitVecToCam), 0.0f), gMaterial.mSpecularSmoothLevel);

		//final SpecularColor
		outSpecular4 = attenuation * SpecFactor * float4(gMaterial.mSpecularColor *lightSpecColor3, 1.0f);
	}

	outColor4 = float4(lerp(outAmbient4.xyz + outDiffuse4.xyz, envMapColor4.xyz, envMapColor4.w), 1.0f) + outSpecular4;
}

/*************************************************
						 PRIVATE 
*************************************************/

bool RenderProcess_PixelLighting::mFunction_ComputeLightingVariables(int lightTypeID, int lightIndex, out float3 lightVecW, out float3 unitLightVecW, out float attenuation, out float3 lightAmbientColor3,out float3 lightDiffuseColor3, out float3	lightSpecColor3, out float lightSpecIntensity)
{
	switch (lightTypeID)
	{
		case NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT:
		{
			//retrieve light parameters
			N_DirectionalLight light = gDirectionalLight_Dynamic[lightIndex];

			//***
			lightVecW = light.mDirection;
			unitLightVecW = normalize(lightVecW);
			attenuation = 0.0f;
			lightAmbientColor3 = light.mAmbientColor;
			lightDiffuseColor3 = light.mDiffuseColor;
			lightSpecColor3 = light.mSpecularColor;
			lightSpecIntensity = light.mSpecularIntensity;
			break;
		}

		case NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT:
		{
			//retrieve light parameters
			N_PointLight light = gPointLight_Dynamic[lightIndex];

			//***lighting vector
			lightVecW = mPosOfCurrentPointW - light.mPosition;
			unitLightVecW = normalize(lightVecW);

			//to check if this point is out of lighting range :: DISTANCE /ANGLE RANGE
			float	distanceFromLight = length(lightVecW);
			if (distanceFromLight > light.mLightingRange) return true; 

			//...
			attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1.0f);
			lightAmbientColor3 = light.mAmbientColor;
			lightDiffuseColor3 = light.mDiffuseColor;
			lightSpecColor3 = light.mSpecularColor;
			lightSpecIntensity = light.mSpecularIntensity;
			break;
		}

		case NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT:
		{
			N_SpotLight light = gSpotLight_Dynamic[lightIndex];

			//...
			lightVecW = mPosOfCurrentPointW - light.mPosition;
			unitLightVecW = normalize(lightVecW);

			//1.the cos(theta) of the angle between direction and current light ray
			//this COSINE is used to jugde if the light ray is in the cone
			//2.check if this point is out of lighting range
			float distanceFromLight = length(lightVecW);
			float Cos_Theta = dot(unitLightVecW, normalize(light.mLitAt - light.mPosition));
			if ((distanceFromLight > light.mLightingRange) || (Cos_Theta < cos(light.mLightingAngle / 2.0f)))return true;

			//...
			attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1.0f);
			lightAmbientColor3 = light.mAmbientColor;
			lightDiffuseColor3 = light.mDiffuseColor;
			lightSpecColor3 = light.mSpecularColor;
			lightSpecIntensity = light.mSpecularIntensity;
			break;
		}
	}

	return false;//false for no return
}

float RenderProcess_PixelLighting::mFunction_ComputeDiffuseCosineFactor(int lightTypeID, int lightIndex, float3 lightVecW, float3 normal)
{
	switch (lightTypeID)
	{
	case NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT:
		return gDirectionalLight_Dynamic[lightIndex].mDiffuseIntensity * dot(-lightVecW, normal);

	case  NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT:
		return gPointLight_Dynamic[lightIndex].mDiffuseIntensity * dot(-lightVecW, normal);

	case NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT:
		return gSpotLight_Dynamic[lightIndex].mDiffuseIntensity * dot(-lightVecW, normal);
	}
}

/*void	ComputeDirLightColor(N_DirectionalLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 tangentW,out float4 outColor4)
{
	outColor4 = float4(0,0,0,1.0f);


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
	

	float4 outAmbient4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 outDiffuse4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 outSpec4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ComputeOutput_Amb_Diff_Spec(diffuseCosFactor,Light.mSpecularIntensity,gMaterial.mSpecularSmoothLevel,1.0f,
									gMaterial.mAmbientColor	,Light.mAmbientColor,
									diffuseColor3						,Light.mDiffuseColor,
									specularColor3					,Light.mSpecularColor,
									Vec_toCamTBN,lightVecTBN,normalTBN,
									outAmbient4,outDiffuse4,outSpec4);
	

	outColor4 = float4(lerp(outAmbient4.xyz + outDiffuse4.xyz,envMapColor4.xyz,envMapColor4.w),1.0f) + outSpec4;
};*/


/*void	ComputePointLightColor(N_PointLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 thisPoint,float3 tangentW,
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
	float distanceFromLight = length(LightVec);
	float Attenuation = 1 / (Light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1);

	
	//to check if this point is out of lighting range :: DISTANCE /ANGLE RANGE
	if(distanceFromLight > Light.mLightingRange)
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
};*/


/*void	ComputeSpotLightColor(N_SpotLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 thisPoint,float3 tangentW,
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
	float distanceFromLight = length(LightVec);
	float Attenuation = 1 / (Light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1);
	
	//the cos(theta) of the angle between direction and current light ray
	//this COSINE is used to jugde if the light ray is in the cone
	float Cos_Theta = dot(Unit_LightVec,normalize(Light.mLitAt - Light.mPosition));
	
	//to check if this point is out of lighting range
	if((distanceFromLight > Light.mLightingRange)||(Cos_Theta < cos(Light.mLightingAngle/2.0f)))
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
};*/