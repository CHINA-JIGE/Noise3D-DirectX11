
/**********************************************

	File: Draw Mesh - per vertex lighting
	Author: Jige
	
	functions invoked by mesh drawing shader
	(every mapping techniques are available)

************************************************/

VS_OUTPUT_DRAW_MESH_GOURAUD VS_DrawMeshWithVertexLighting(VS_INPUT_DRAW_MESH input)
{
	//initialize
	VS_OUTPUT_DRAW_MESH_GOURAUD output = (VS_OUTPUT_DRAW_MESH_GOURAUD)0;
	//the W transformation
	output.posW = mul(float4(input.posL, 1.0f), gWorldMatrix).xyz;
	//the VP transformation
	output.posH = mul(mul(float4(output.posW, 1.0f), gViewMatrix), gProjMatrix);
	//we need an normal vector in W space
	output.normalW = mul(float4(input.normalL, 1.0f), gWorldInvTransposeMatrix).xyz;
	//texture coordinate
	output.texcoord = input.texcoord;

	//vertex lighting
	float4 	ambientLightingColor = float4(0, 0, 0, 0);
	float4 	diffuseLightingColor = float4(0, 0, 0, 0);
	float4  specularLightingColor = float4(0, 0, 0, 0);
	RenderProcess_Gouraud renderProc;
	renderProc.ComputeLightingColorForVS(output.normalW, output.posW, ambientLightingColor, diffuseLightingColor, specularLightingColor);

	//because in Pixel shader, diffuse color need to be mul by albedo/diffuseMapColor
	//hence amb/diff/spec are passed to PS seperately
	output.ambient = ambientLightingColor;
	output.diffuse = diffuseLightingColor;
	output.specular = specularLightingColor;

	return output;
}

PS_OUTPUT_DRAW_MESH PS_DrawMeshWithVertexLighting(VS_OUTPUT_DRAW_MESH_GOURAUD input, uniform bool bDiffMap)
{
	float4 	outColor4 = float4(0, 0, 0, 0);
	RenderProcess_Gouraud renderProc;
	renderProc.ComputeFinalColorForPS(
		input.normalW,
		input.posW,
		input.texcoord,
		input.ambient,
		input.diffuse,
		input.specular,
		bDiffMap,
		outColor4);


	PS_OUTPUT_DRAW_MESH psOutput = (PS_OUTPUT_DRAW_MESH)0;
	psOutput.color = outColor4;
	return psOutput;
}



void RenderProcess_Gouraud::ComputeLightingColorForVS(float3 normalW, float3 posW, 
	out float4 outAmbient4, out float4 outDiffuse4, out float4 outSpecular4)
{
	//init output color
	outAmbient4 = float4(0, 0, 0, 0);
	outDiffuse4 = float4(0, 0, 0, 0);
	outSpecular4 = float4(0, 0, 0, 0);

	//init geometry info
	mNormalW = normalW;
	mPosW = posW;
	mVecToCamW = gCamPos - posW;

	//sum up all the lighting contribution into output
	int i = 0;

	//[unroll]
	if (gIsLightingEnabled_Dynamic)
	{
		for (i = 0; i<gDirectionalLightCount_Dynamic; i++)
		{
			float4 amb = float4(0, 0, 0, 0);
			float4 diff = float4(0, 0, 0, 0);
			float4 spec = float4(0, 0, 0, 0);
			RenderProcess_Gouraud::mFunction_ComputeLightColor(NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT, i, amb, diff, spec);
			outAmbient4	+= amb;
			outDiffuse4		+= diff;
			outSpecular4	+= spec;
		}
		for (i = 0; i<gPointLightCount_Dynamic; i++)
		{
			float4 amb = float4(0, 0, 0, 0);
			float4 diff = float4(0, 0, 0, 0);
			float4 spec = float4(0, 0, 0, 0);
			RenderProcess_Gouraud::mFunction_ComputeLightColor(NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT, i, amb, diff, spec);
			outAmbient4 += amb;
			outDiffuse4 += diff;
			outSpecular4 += spec;
		}
		for (i = 0; i<gSpotLightCount_Dynamic; i++)
		{
			float4 amb = float4(0, 0, 0, 0);
			float4 diff = float4(0, 0, 0, 0);
			float4 spec = float4(0, 0, 0, 0);
			RenderProcess_Gouraud::mFunction_ComputeLightColor(NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT, i, amb, diff, spec);
			outAmbient4 += amb;
			outDiffuse4 += diff;
			outSpecular4 += spec;
		}
	}

	outAmbient4.w = 1.0f;
	outDiffuse4.w =1.0f;
	outSpecular4.w = 1.0f;
}

void RenderProcess_Gouraud::ComputeFinalColorForPS(float3 normalW, float3 posW, float2 texcoord, 
	float4 ambient, float4 diffuse, float4 specular, uniform bool bDiffMap, out float4 outColor4)
{
	//init geometry info
	mNormalW = normalW;
	mPosW = posW;
	mVecToCamW = gCamPos - posW;

	//FOG: skip pixel if it is totally fogged
	if (gFogEnabled == 1 && length(mVecToCamW) > gFogFar)
	{
		outColor4 = float4(gFogColor3, 1.0f);
		return;
	}

	//albedo is a measure of diffusive reflection 
	//(2018.1.15)i assume that albedo/diffuseMap only affect diffuse lighting color
	float3 albedo3	= SampleFromDiffuseMap(texcoord, bDiffMap);
	outColor4 = float4(ambient.xyz + albedo3 * diffuse.xyz + specular.xyz, gMaterial.mTransparency);

	//compute Fog effect
	//at last compute fog  ( point farther than gFogFar has been skipped
	if (gFogEnabled)
	{
		float fogInterpolationFactor = max(0, (length(mVecToCamW) - gFogNear) / (gFogFar - gFogNear));
		outColor4 = lerp(outColor4, float4(gFogColor3, 1.0f), fogInterpolationFactor);
	}

}


//****************Internally invoked**********************
//color compute for dynamic/static -- dir/point/spot light
void RenderProcess_Gouraud::mFunction_ComputeLightColor(int lightTypeID, int lightIndex, out float4 outAmbient4, out float4 outDiffuse4,out float4 outSpecular4)
{
	outAmbient4 = float4(0, 0, 0, 0);
	outDiffuse4 = float4(0, 0, 0, 0);
	outSpecular4 = float4(0, 0, 0, 0);

	float3	lightVecW = float3(0, 0, 0);
	float3	unitLightVecW = float3(0, 0, 0);
	float		attenuation = 0.0f;
	float3	lightAmbientColor3 = float3(0, 0, 0);
	float3	lightDiffuseColor3 = float3(0, 0, 0);
	float3	lightSpecColor3 = float3(0, 0, 0);
	float		lightSpecIntensity = 0.0f;

	//----------compute LightVec/Attenuation------------
	bool bReturn = mFunction_ComputeLightingVariables(
		lightTypeID, lightIndex, lightVecW, unitLightVecW, attenuation,
		lightAmbientColor3, lightDiffuseColor3, lightSpecColor3, lightSpecIntensity);

	if (bReturn)return;

	//diffuse cos factor
	float diffuseCosFactor = RenderProcess_Gouraud::mFunction_ComputeDiffuseCosineFactor(lightTypeID, lightIndex, unitLightVecW, normalize(mNormalW));

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient4 = float4(gMaterial.mAmbientColor*lightAmbientColor3, 1.0f);

	//if this pixel can be lit .... and this judgement is some sort of optimization
	if (diffuseCosFactor > 0.0f)
	{
		//!!!!!!!!material/diffuse mapping value is NOT considered in here
		outDiffuse4 = attenuation* diffuseCosFactor* float4(gMaterial.mDiffuseColor*lightDiffuseColor3, 1.0f);

		//now specular	----reflect () : input an incoming light,and output an outgoing light , the axis being reflected about is a bisecting vector of NORMAL and LIGHTVEC
		float3 reflectedLightVec = reflect(unitLightVecW, normalize(mNormalW));

		//remember to normalize vectors to be "dotted"
		float3 unitVecToCam = normalize(mVecToCamW);

		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = lightSpecIntensity * pow(max(dot(reflectedLightVec, unitVecToCam), 0.0f), gMaterial.mSpecularSmoothLevel);

		//final SpecularColor
		outSpecular4 = attenuation * SpecFactor * float4(gMaterial.mSpecularColor *lightSpecColor3, 1.0f);
	}
}

bool RenderProcess_Gouraud::mFunction_ComputeLightingVariables(int lightTypeID, int lightIndex, 
	out float3 lightVecW, out float3 unitLightVecW, out float attenuation, out float3 lightAmbientColor3, 
	out float3 lightDiffuseColor3, out float3 lightSpecColor3, out float lightSpecIntensity)
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
			lightVecW = mPosW - light.mPosition;
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
			lightVecW = mPosW - light.mPosition;
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

float RenderProcess_Gouraud::mFunction_ComputeDiffuseCosineFactor(int lightTypeID, int lightIndex, float3 lightVecW, float3 normalW)
{
	switch (lightTypeID)
	{
		case NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT:
		{
			return gDirectionalLight_Dynamic[lightIndex].mDiffuseIntensity * dot(-lightVecW, normalW);
			break;
		}

		case  NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT:
		{
			return gPointLight_Dynamic[lightIndex].mDiffuseIntensity * dot(-lightVecW, normalW);
			break;
		}

		case NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT:
		{
			return gSpotLight_Dynamic[lightIndex].mDiffuseIntensity *dot(-lightVecW, normalW);
			break;
		}
	}
}
