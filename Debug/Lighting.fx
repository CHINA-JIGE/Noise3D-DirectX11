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
	float3 mSpecularColor;		float mAttenuationFactor;
	float3 mPosition;				float mDiffuseIntensity;//memory alignment
};

struct N_SpotLight
{
	float3 mAmbientColor;			float mSpecularIntensity;
	float3 mDiffuseColor;			float mLightingRange;
	float3 mSpecularColor;		float mAttenuationFactor;
	float3 mLitAt;					float mLightingAngle;
	float3 mPosition;				float mDiffuseIntensity;//memory alignment
};

struct N_Material
{
	float3	mAmbientColor;		int		mSpecularSmoothLevel;
	float3	mDiffuseColor;		float		mPad1;
	float3	mSpecularColor;		float		mPad2;
};

void	ComputeDirLightColor(N_Material Mat,N_DirectionalLight Light,float3 Normal,float3 Vec_toEye,
							 out float4 outAmbient, out float4 outDiffuse, out float4 outSpec)
{
	outAmbient	= float4(0.0f,0.0f,0.0f,0.0f);
	outDiffuse	= float4(0.0f,0.0f,0.0f,0.0f);
	outSpec		= float4(0.0f,0.0f,0.0f,0.0f);

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient	= float4(Mat.mAmbientColor*Light.mAmbientColor,1.0f);

	//direction of the incoming light
	float3	LightVec = normalize(Light.mDirection);
	
	//Diffuse factor :: the Cos(theta)
	float 	diffuseCosFactor = Light.mDiffuseIntensity *dot(-LightVec,Normal);
	
	[flatten]
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if(diffuseCosFactor > 0.0f)
	{
		//final Diffuse
		outDiffuse = diffuseCosFactor* float4(Mat.mDiffuseColor * Light.mDiffuseColor,1.0f);
		
		//now specular	----reflect () : input an incoming light,and output an outgoing light
		float3 tmpV = reflect(LightVec,Normal);
		
		//remember to normalize vectors to be "dotted"
		Vec_toEye = normalize(Vec_toEye);
		
		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = Light.mSpecularIntensity * pow(max(dot(tmpV,Vec_toEye),0.0f),Mat.mSpecularSmoothLevel);
		
		//final SpecularColor
		outSpec = SpecFactor * float4(Mat.mSpecularColor * Light.mSpecularColor,0.0f);
	}
	
};


void	ComputePointLightColor(N_Material Mat,N_PointLight Light,float3 Normal,float3 Vec_toEye,float3 thisPoint,
							 out float4 outAmbient, out float4 outDiffuse, out float4 outSpec)
{
	outAmbient	= float4(0.0f,0.0f,0.0f,0.0f);
	outDiffuse	= float4(0.0f,0.0f,0.0f,0.0f);
	outSpec		= float4(0.0f,0.0f,0.0f,0.0f);

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient	= float4(Mat.mAmbientColor*Light.mAmbientColor,1.0f);

	// direction of the incoming light
	float3	LightVec = thisPoint - Light.mPosition;
	float3	Unit_LightVec = normalize(LightVec);

	//.............
	float disFromLight = length(LightVec);
	float Attenuation = 1 / (Light.mAttenuationFactor*disFromLight*disFromLight + 1);

	
	//to check if this point is out of lighting range :: DISTANCE /ANGLE RANGE
	if(disFromLight > Light.mLightingRange)
	{
		return;
	}

	//Diffuse factor ::  Cos(theta)
	float diffuseCosFactor =Light.mDiffuseIntensity * dot(-Unit_LightVec,Normal);
	
	[flatten]
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if(diffuseCosFactor > 0.0f)
	{
		//final Diffuse
		outDiffuse = Attenuation * diffuseCosFactor* float4(Mat.mDiffuseColor * Light.mDiffuseColor,1.0f);
		
		//now specular	----reflect () : input an incoming light,and output an outgoing light
		float3 tmpV = reflect(Unit_LightVec,Normal);
		
		//remember to normalize vectors to be "dotted"
		Vec_toEye = normalize(Vec_toEye);


		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = Light.mSpecularIntensity * pow(max(dot(tmpV,Vec_toEye),0.0f),Mat.mSpecularSmoothLevel);
		
		//final SpecularColor
		outSpec = Attenuation * SpecFactor * float4(Mat.mSpecularColor * Light.mSpecularColor,0.0f);
	}
};


void	ComputeSpotLightColor(N_Material Mat,N_SpotLight Light,float3 Normal,float3 Vec_toEye,float3 thisPoint,
							 out float4 outAmbient, out float4 outDiffuse, out float4 outSpec)
{
	outAmbient	= float4(0.0f,0.0f,0.0f,0.0f);
	outDiffuse	= float4(0.0f,0.0f,0.0f,0.0f);
	outSpec		= float4(0.0f,0.0f,0.0f,0.0f);

	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient	= float4(Mat.mAmbientColor*Light.mAmbientColor,1.0f);

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
		return;
	}

	//Diffuse factor ::  Cos(theta)
	float diffuseCosFactor = Light.mDiffuseIntensity *dot(-Unit_LightVec,Normal);
	
	[flatten]
	//if this pixel can be lit .... and this judgement is some sort of optimization
	if(diffuseCosFactor > 0.0f)
	{
		//final Diffuse
		outDiffuse = Attenuation * diffuseCosFactor* float4(Mat.mDiffuseColor * Light.mDiffuseColor,1.0f);
		
		//now specular	----reflect () : input an incoming light,and output an outgoing light
		float3 tmpV = reflect(Unit_LightVec,Normal);
		
		//remember to normalize vectors to be "dotted"
		Vec_toEye = normalize(Vec_toEye);
		
		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = Light.mSpecularIntensity * pow(max(dot(tmpV,Vec_toEye),0.0f),Mat.mSpecularSmoothLevel);
		
		//final SpecularColor
		outSpec = Attenuation * SpecFactor * float4(Mat.mSpecularColor * Light.mSpecularColor,0.0f);
	}
};