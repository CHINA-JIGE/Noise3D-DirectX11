
/**********************************************

	File: Draw Mesh - per pixel lighting
	Author: Jige
	
	functions invoked by mesh drawing shader
	(every mapping techniques are available)

************************************************/

VS_OUTPUT_DRAW_MESH_PHONG VS_DrawMeshWithPixelLighting(VS_INPUT_DRAW_MESH input)
{
	//initialize
	VS_OUTPUT_DRAW_MESH_PHONG output = (VS_OUTPUT_DRAW_MESH_PHONG)0;
	//the W transformation
	output.posW = mul(float4(input.posL, 1.0f), gWorldMatrix).xyz;
	//the VP transformation
	output.posH = mul(mul(float4(output.posW, 1.0f), gViewMatrix), gProjMatrix);
	//output the vertex color , this parameter will be used if the lighting system is off
	output.color = input.color;
	//we need an normal vector in W space(it can be derived that inverse-transpose guaranteed the correct transform of normal)
	output.normalW = mul(float4(input.normalL, 1.0f), gWorldInvTransposeMatrix).xyz; //mul(float4(input.normalL, 0.0f), gWorldMatrix).xyz;
	//transform tangent to help implement XYZ to TBN
	output.tangentW = mul(float4(input.tangentL, 0.0f), gWorldMatrix).xyz;
	//texture coordinate
	output.texcoord = input.texcoord;

	return output;
}

PS_OUTPUT_DRAW_MESH PS_DrawMeshWithPixelLighting(VS_OUTPUT_DRAW_MESH_PHONG input,
	uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap)
{
	//the output
	PS_OUTPUT_DRAW_MESH psOutput = (PS_OUTPUT_DRAW_MESH)0;

	//if dynamic lighting is disable, then use vertex color
	if (!gIsLightingEnabled_Dynamic)
	{
		psOutput.color = input.color;
		return psOutput;//vertex color
	}
	//interpolation can  'unnormalized' the unit  vector
	input.normalW = normalize(input.normalW);
	input.tangentW = normalize(input.tangentW);

	//invoke the process of lighting/mapping/shading
	float4 	finalColor4 = float4(0, 0, 0, 0);
	RenderProcess_Phong renderProc;
	renderProc.InitVectors(input.normalW, input.texcoord, input.posW, input.tangentW);
	renderProc.ComputeFinalColor(bDiffMap, bNormalMap, bSpecMap, bEnvMap, finalColor4);

	psOutput.color = finalColor4;
	return psOutput;
}



//-------------CLASS IMPLEMENTATION--------

void RenderProcess_Phong::InitVectors(float3 NormalW, float2 TexCoord, float3 thisPoint, float3 tangentW)
{
	mNormalW = NormalW;//surface normal of current point
	mTexcoord = TexCoord;
	mVecToCamW = gCamPos - thisPoint;//vector from current point to the camera position
	mPosOfCurrentPointW = thisPoint;
	mTangentW = tangentW;
}

void RenderProcess_Phong::ComputeFinalColor(uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap,out float4 outColor4)
{
	outColor4 = float4(0, 0, 0, 1.0f);



	//compute fog effect
	//skip pixel if it is totally fogged
	if (gFogEnabled == 1 && length(mVecToCamW) > gFogFar)
	{
		outColor4 = float4(gFogColor3, 1.0f);
		return;
	}

	//sum up all the lighting contribution 'tmpColor4' into 'finalColor4'
	int i = 0;
	for (i = 0; i<gDirectionalLightCount_Dynamic; i++)
	{
		float4 tmpColor4 = float4(0, 0, 0, 0);
		mFunction_ComputeLightColor(NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT, i, bDiffMap, bNormalMap, bSpecMap, bEnvMap, tmpColor4);
		outColor4 += tmpColor4;
	}
	for (i = 0; i<gPointLightCount_Dynamic; i++)
	{
		float4 tmpColor4 = float4(0, 0, 0, 0);
		mFunction_ComputeLightColor(NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT, i, bDiffMap, bNormalMap, bSpecMap, bEnvMap, tmpColor4);
		outColor4 += tmpColor4;
	}
	for (i = 0; i<gSpotLightCount_Dynamic; i++)
	{
		float4 tmpColor4 = float4(0, 0, 0, 0);
		mFunction_ComputeLightColor(NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT, i, bDiffMap, bNormalMap, bSpecMap, bEnvMap, tmpColor4);
		outColor4 += tmpColor4;
	}

	//at last compute fog  ( point farther than gFogFar has been skipped
	if (gFogEnabled)
	{
		float fogInterpolationFactor = max(0, (length(mVecToCamW) - gFogNear) / (gFogFar - gFogNear));
		outColor4 = lerp(outColor4, float4(gFogColor3, 1.0f), fogInterpolationFactor);
	}

	//set transparency component
	outColor4.w = saturate(gMaterial.mTransparency);
}

/*************************************************
								PRIVATE 
*************************************************/

void RenderProcess_Phong::mFunction_ComputeLightColor(int lightTypeID, int lightIndex, uniform bool bDiffMap, uniform bool bNormalMap, uniform bool bSpecMap, uniform bool bEnvMap, out float4 outColor4)
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
    bool bReturn = RenderProcess_Phong::mFunction_ComputeLightingVariables(
		lightTypeID, lightIndex,  lightVecW, unitLightVecW,  attenuation, 
        lightAmbientColor3, lightDiffuseColor3, lightSpecColor3,lightSpecIntensity);

	if (bReturn)return;

	//----------Sample from maps----------
	//albedo is a measure for diffusive reflection
	float3 albedo3 = float3(0, 0, 0);
	float3 normalTBN = float3(0, 1.0f, 0);
	float3 specularColor3 = float3(0, 0, 0);
	float4 envMapColor4 = float4(0, 0, 0, 0);
	float3 deviatedNormalW = float3(0, 0, 0);
	float3 lightVecTBN = float3(0, 0, 0);
	float3 Vec_toCamTBN = float3(0, 0, 0);
	float   diffuseCosFactor = 0.0f;

	albedo3	=	SampleFromDiffuseMap(mTexcoord, bDiffMap);
	normalTBN		=	SampleFromNormalMap(mTexcoord, bNormalMap);
	specularColor3 = SampleFromSpecularMap(mTexcoord, bSpecMap);
	TransformCoord_TBN_XYZ(normalTBN, mTangentW, mNormalW, deviatedNormalW);
	envMapColor4	= SampleFromEnvironmentMap(mVecToCamW, deviatedNormalW, bEnvMap);//we must get the alpha to blend
	TransformCoord_XYZ_TBN(mVecToCamW, mTangentW, mNormalW, Vec_toCamTBN);
	TransformCoord_XYZ_TBN(unitLightVecW, mTangentW, mNormalW, lightVecTBN);

    //diffuse cos factor
	diffuseCosFactor = mFunction_ComputeDiffuseCosineFactor(lightTypeID, lightIndex, unitLightVecW, normalize(deviatedNormalW));
	//diffuseCosFactor = mFunction_ComputeDiffuseCosineFactor(lightTypeID, lightIndex, lightVecTBN, normalize(normalTBN));


    float4 ambient4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular4 = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	ambient4 = float4(gMaterial.mAmbientColor*lightAmbientColor3, 1.0f);

	//if this pixel can be lit .... and this judgement is some sort of optimization
	if (diffuseCosFactor > 0.0f)
	{
		diffuse4 = attenuation* diffuseCosFactor* float4(albedo3 * lightDiffuseColor3, 1.0f);

		//now specular	----reflect () : input an incoming light,and output an outgoing light , the axis being reflected about is a bisecting vector of NORMAL and LIGHTVEC
		float3 reflectedLightVec = reflect(unitLightVecW, normalize(deviatedNormalW));

		//remember to normalize vectors to be "dotted"
		float3 unitVecToCam = normalize(mVecToCamW);

		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = lightSpecIntensity * pow(max(dot(reflectedLightVec, unitVecToCam), 0.0f), gMaterial.mSpecularSmoothLevel);

		//final SpecularColor
		specular4 = attenuation * SpecFactor * float4(gMaterial.mSpecularColor *lightSpecColor3, 1.0f);
	}

	//environment mapping is using GRADIENT-BASED operations
	outColor4 = float4(lerp(ambient4.xyz + diffuse4.xyz, envMapColor4.xyz, envMapColor4.w)+ specular4.xyz, 1.0f) ;
	//outColor4 = float4(ambient4.xyz + diffuse4.xyz+ specular4.xyz+envMapColor4.xyz, 1.0f) ;
}

bool RenderProcess_Phong::mFunction_ComputeLightingVariables(int lightTypeID, int lightIndex, out float3 lightVecW, out float3 unitLightVecW, out float attenuation, out float3 lightAmbientColor3,out float3 lightDiffuseColor3, out float3	lightSpecColor3, out float lightSpecIntensity)
{
	lightVecW = float3(0,0,0);
	unitLightVecW = float3(0,0,0);
	attenuation = 0.0f;
	lightAmbientColor3 = float3(0,0,0);
	lightDiffuseColor3 = float3(0, 0, 0);
	lightSpecColor3 = float3(0, 0, 0);
	lightSpecIntensity = 1.0f;

	switch (lightTypeID)
	{
		case NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT:
		{
			//retrieve light parameters
			N_DirectionalLight light = gDirectionalLight_Dynamic[lightIndex];

			//***
			lightVecW = light.mDirection;
			unitLightVecW = normalize(lightVecW);
			attenuation = 1.0f;
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
			//attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1.0f);
			attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight + 1.0f);
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
			//attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight*distanceFromLight + 1.0f);
			attenuation = 1.0f / (light.mAttenuationFactor*distanceFromLight + 1.0f);
			lightAmbientColor3 = light.mAmbientColor;
			lightDiffuseColor3 = light.mDiffuseColor;
			lightSpecColor3 = light.mSpecularColor;
			lightSpecIntensity = light.mSpecularIntensity;
			break;
		}
	}

	//false for indicating the continuation of following shading process
	//true for skipping
	return false;
}

float RenderProcess_Phong::mFunction_ComputeDiffuseCosineFactor(int lightTypeID, int lightIndex, float3 lightVecW, float3 normal)
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
