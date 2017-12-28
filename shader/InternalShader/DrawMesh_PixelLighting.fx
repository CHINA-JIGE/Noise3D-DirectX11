
/**********************************************

	File: Draw Mesh - per pixel lighting
	Author: Jige
	
	functions invoked by mesh drawing shader
	(every mapping techniques are available)

************************************************/

/**********************************************************************
					SHADING FUNCTION
**************************************************************************/
//function used by all light-computing functions
void	ComputeOutput_Amb_Diff_Spec(float diffuseCosFactor, float lightSpecIntensity, 
										float matSpecSmoothLevel, float Attenuation,
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


void RenderProcess_PixelLighting::ComputeLightColor(int lightTypeID, int lightIndex,out float4 outColor4)
{
	outColor4 = float4(0, 0, 0, 1.0f);
	float3	lightVec = float3(0, 0, 0);
	float3	unitLightVec = float3(0,0,0);
	float		Attenuation = 0.0f;
	float3	lightAmbientColor3 = float3(0,0,0);
	float3	lightDiffuseColor3 = float3(0, 0, 0);
	float3	lightSpecColor3 = float3(0, 0, 0);

	//----------compute LightVec/Attenuation------------
	switch (lightTypeID)
	{
		case NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT:
		case NOISE_LIGHT_TYPE_ID_STATIC_DIR_LIGHT:
		{
			//retrieve light parameters
			N_DirectionalLight light= (N_DirectionalLight)0;
			if (lightTypeID == NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT)
				light = gDirectionalLight_Dynamic[lightIndex];
			else if (lightTypeID == NOISE_LIGHT_TYPE_ID_STATIC_DIR_LIGHT)
				light = gDirectionalLight_Static[lightIndex];

			//lighting vector
			lightVec = light.mDirection;
			unitLightVec = normalize(lightVec);
			Attenuation = 0.0f;
			lightAmbientColor3 = light.mAmbientColor;
			lightDiffuseColor3 = light.mDiffuseColor;
			lightSpecColor3 = light.mSpecColor;
			break;
		}

		case NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT:
		case	NOISE_LIGHT_TYPE_ID_STATIC_POINT_LIGHT:
		{
			//retrieve light parameters
			N_PointLight light = (N_PointLight)0;
			if (lightTypeID == NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT)
				light = gPointLight_Dynamic[lightIndex];
			else if (lightTypeID == NOISE_LIGHT_TYPE_ID_STATIC_POINT_LIGHT)
				light = gPointLight_Static[lightIndex];

			//lighting vector
			lightVec = mPosOfCurrentPointW - light.mPosition;
			unitLightVec = normalize(lightVec);

			//to check if this point is out of lighting range :: DISTANCE /ANGLE RANGE
			float	distanceFromLight = length(lightVec);
			Attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1.0f);
			if (distanceFromLight > light.mLightingRange)
			{
				outColor4 = float4(0, 0, 0, 1.0f);
				return;
			}
			break;
		}

		case NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT:
		case	NOISE_LIGHT_TYPE_ID_STATIC_SPOT_LIGHT:
		{
			N_SpotLight light = (N_SpotLight)0;
			if (lightTypeID == NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT)
				light = gSpotLight_Dynamic[lightIndex];
			else if (lightTypeID == NOISE_LIGHT_TYPE_ID_STATIC_SPOT_LIGHT)
				light = gSpotLight_Static[lightIndex];

			lightVec = mPosOfCurrentPointW - light.mPosition;
			unitLightVec = normalize(lightVec);
			float distanceFromLight = length(lightVec);
			Attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1.0f);

			//the cos(theta) of the angle between direction and current light ray
			//this COSINE is used to jugde if the light ray is in the cone
			float Cos_Theta = dot(unitLightVec, normalize(light.mLitAt - light.mPosition));

			//to check if this point is out of lighting range
			if ((distanceFromLight > light.mLightingRange) || (Cos_Theta < cos(light.mLightingAngle / 2.0f)))
			{
				outColor4 = float4(0, 0, 0, 1.0f);
				return;
			}
			break;
		}
	}


	//----------Sample from maps----------
	float3 diffuseColor3 = float3(0, 0, 0);
	float3 normalTBN = float3(0, 1.0f, 0);
	float3 specularColor3 = float3(0, 0, 0);
	float4 envMapColor4 = float4(0, 0, 0, 0);
	float3 deviatedNormalW = float3(0, 0, 0);
	float3 lightVecTBN = float3(0, 0, 0);
	float3 Vec_toCamTBN = float3(0, 0, 0);

	diffuseColor3	=	SampleFromDiffuseMap(mTexcoord, mEnableDiffuseMap);
	normalTBN		=	SampleFromNormalMap(mTexcoord, mEnableNormalMap);
	specularColor3 = SampleFromSpecularMap(mTexcoord, mEnableSpecularMap);
	TransformCoord_TBN_XYZ(normalTBN, tangentW, NormalW, deviatedNormalW);
	envMapColor4	= SampleFromEnvironmentMap(mVecToCamW, envMapColor4);//we must get the alpha to blend
	TransformCoord_XYZ_TBN(Vec_toCam, tangentW, NormalW, Vec_toCamTBN);
	TransformCoord_XYZ_TBN(Unit_LightVec, tangentW, NormalW, lightVecTBN);

	float diffuseCosFactor = Light.mDiffuseIntensity *dot(-lightVecTBN, normalTBN);
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