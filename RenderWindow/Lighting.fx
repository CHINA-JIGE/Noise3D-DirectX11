/**********************************************************************

	File:Lighting.fx
	Author: Jige

**************************************************************************/

struct N_DirectionalLight
{
	 float3 mAmbientColor;		float		mSpecularIntensity;
	 float3 mDiffuseColor;		float		mDiffuseIntensity;//memory alignment
	 float3 mSpecularColor;		float		mPad2;//memory alignment
	 float3 mDirection;			float		mPad3;//memory alignment

};

struct N_PointLight
{
	float3 mAmbientColor;			float mSpecularIntensity;
	float3 mDiffuseColor;			float mLightingRange;
	float3 mSpecularColor;			float mAttenuationFactor;
	float3 mPosition;				float mDiffuseIntensity;//memory alignment
};

struct N_SpotLight
{
	float3 mAmbientColor;			float mSpecularIntensity;
	float3 mDiffuseColor;			float mLightingRange;
	float3 mSpecularColor;			float mAttenuationFactor;
	float3 mLitAt;					float mLightingAngle;
	float3 mPosition;				float mDiffuseIntensity;//memory alignment
};

struct N_Material_Basic
{
	float3	mAmbientColor;		int		mSpecularSmoothLevel;
	float3	mDiffuseColor;		float		mPad1;
	float3	mSpecularColor;		float		mPad2;
};


Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSpecularMap;

SamplerState sampler_ANISOTROPIC
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};




void	ComputeDirLightColor(N_Material_Basic Mat,N_DirectionalLight Light,float3 Normal,float3 Vec_toEye,
							bool DiffuseMapEnabled,bool NormalMapEnabled,bool SpecularMapEnabled,
							Texture2D diffuseMap, Texture2D normalMap, Texture2D specularMap,
							float2 TexCoord, out float4 outColor4)
{
	outColor4 = float4(0,0,0,1.0f);
	float4 outAmbient4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outDiffuse4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outSpec4		= float4(0.0f,0.0f,0.0f,0.0f);


	//direction of the incoming light
	float3	LightVec = normalize(Light.mDirection);
	
	//Diffuse factor :: the Cos(theta)
	float 	diffuseCosFactor = Light.mDiffuseIntensity *dot(-LightVec,Normal);
	
	//let's see if diffuse map is valid, and determine whether to use diffuse map
	float3 diffuseColor3 = float3(0, 0, 0);
	if (DiffuseMapEnabled)
	{
		diffuseColor3 = diffuseMap.Sample(sampler_ANISOTROPIC, TexCoord).xyz;
	}
	else
	{
		//invalid diffuse map, we should use pure color of basic material
		diffuseColor3 = Mat.mDiffuseColor;
	}


	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient4 = float4(Mat.mAmbientColor*Light.mAmbientColor*diffuseColor3, 1.0f);


	[flatten]
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if(diffuseCosFactor > 0.0f)
	{

		outDiffuse4 = diffuseCosFactor* float4(diffuseColor3 * Light.mDiffuseColor,1.0f);
		
		//now specular	----reflect () : input an incoming light,and output an outgoing light
		float3 tmpV = reflect(LightVec,Normal);
		
		//remember to normalize vectors to be "dotted"
		Vec_toEye = normalize(Vec_toEye);
		
		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = Light.mSpecularIntensity * pow(max(dot(tmpV,Vec_toEye),0.0f),Mat.mSpecularSmoothLevel);
		
		//final SpecularColor
		outSpec4 = SpecFactor * float4(Mat.mSpecularColor * Light.mSpecularColor,0.0f);
	}
	
	outColor4 = outAmbient4 + outDiffuse4 + outSpec4;
};



void	ComputePointLightColor(N_Material_Basic Mat,N_PointLight Light,float3 Normal,float3 Vec_toEye,float3 thisPoint,
								bool DiffuseMapEnabled,bool NormalMapEnabled,bool SpecularMapEnabled,
								Texture2D diffuseMap,Texture2D normalMap,Texture2D specularMap,
								float2 TexCoord, out float4 outColor4)
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

	//Diffuse factor ::  Cos(theta)
	float diffuseCosFactor =Light.mDiffuseIntensity * dot(-Unit_LightVec,Normal);
	
	//let's see if diffuse map is valid, and determine whether to use diffuse map
	float3 diffuseColor3 = float3(0, 0, 0);
	if (DiffuseMapEnabled)
	{
		diffuseColor3 = diffuseMap.Sample(sampler_ANISOTROPIC, TexCoord).xyz;
	}
	else
	{
		//invalid diffuse map, we should use pure color of basic material
		diffuseColor3 = Mat.mDiffuseColor;
	}

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient4 = float4(Mat.mAmbientColor*Light.mAmbientColor*diffuseColor3, 1.0f);


	[flatten]
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if(diffuseCosFactor > 0.0f)
	{

		//final Diffuse
		outDiffuse4 = Attenuation * diffuseCosFactor* float4(diffuseColor3 * Light.mDiffuseColor,1.0f);

		//now specular	----reflect () : input an incoming light,and output an outgoing light
		float3 tmpV = reflect(Unit_LightVec,Normal);
		
		//remember to normalize vectors to be "dotted"
		Vec_toEye = normalize(Vec_toEye);


		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = Light.mSpecularIntensity * pow(max(dot(tmpV,Vec_toEye),0.0f),Mat.mSpecularSmoothLevel);
		
		//final SpecularColor
		outSpec4 = Attenuation * SpecFactor * float4(Mat.mSpecularColor * Light.mSpecularColor,0.0f);
	}

	outColor4 = outAmbient4 + outDiffuse4 + outSpec4;
};



void	ComputeSpotLightColor(N_Material_Basic Mat,N_SpotLight Light,float3 Normal,float3 Vec_toEye,float3 thisPoint,
								bool DiffuseMapEnabled,bool NormalMapEnabled,bool SpecularMapEnabled,
								Texture2D diffuseMap,Texture2D normalMap,Texture2D specularMap,
								float2 TexCoord, out float4 outColor4)
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
	if((disFromLight > Light.mLightingRange)||(Cos_Theta < cos(Light.mLightingAngle)))
	{
		outColor4 = float4(0,0,0,1.0f);
		return;
	}

	//Diffuse factor ::  Cos(theta)
	float diffuseCosFactor = Light.mDiffuseIntensity *dot(-Unit_LightVec,Normal);
	
	//let's see if diffuse map is valid, and determine whether to use diffuse map
	float3 diffuseColor3 = float3(0, 0, 0);
	if (DiffuseMapEnabled)
	{
		diffuseColor3 = diffuseMap.Sample(sampler_ANISOTROPIC, TexCoord).xyz;
	}
	else
	{
		//invalid diffuse map, we should use pure color of basic material
		diffuseColor3 = Mat.mDiffuseColor;
	}

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient4 = float4(Mat.mAmbientColor*Light.mAmbientColor*diffuseColor3, 1.0f);




	[flatten]
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if(diffuseCosFactor > 0.0f)
	{

		//final Diffuse
		outDiffuse4 = Attenuation * diffuseCosFactor* float4(diffuseColor3 * Light.mDiffuseColor,1.0f);

		//now specular	----reflect () : input an incoming light,and output an outgoing light
		float3 tmpV = reflect(Unit_LightVec,Normal);
		
		//remember to normalize vectors to be "dotted"
		Vec_toEye = normalize(Vec_toEye);
		
		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = Light.mSpecularIntensity * pow(max(dot(tmpV,Vec_toEye),0.0f),Mat.mSpecularSmoothLevel);
		
		//final SpecularColor
		outSpec4 = Attenuation * SpecFactor * float4(Mat.mSpecularColor * Light.mSpecularColor,0.0f);
	}

	outColor4 = outAmbient4 + outDiffuse4 + outSpec4;
};