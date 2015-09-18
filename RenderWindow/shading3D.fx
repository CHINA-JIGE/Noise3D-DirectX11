/**********************************************************************

	File:shading3D.fx
	Author: Jige

**************************************************************************/




/**********************************************************************
			 STRUCTURE    DEFINITION
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
	float3	mDiffuseColor;			float		mPad1;
	float3	mSpecularColor;		float		mPad2;
};






/**********************************************************************
			 CONSTANT    BUFFER
**************************************************************************/
cbuffer cbPerObject
{
	float4x4	gWorldMatrix;
	float4x4	gWorldInvTransposeMatrix;
};

cbuffer cbPerFrame
{
	float4x4				gProjMatrix;
	float4x4				gViewMatrix;
	//！！！！Dynamic Light！！！！
	N_DirectionalLight gDirectionalLight_Dynamic[10];
	N_PointLight	 gPointLight_Dynamic[10];
	N_SpotLight	gSpotLight_Dynamic[10];
	int		gDirectionalLightCount_Dynamic;
	int		gPointLightCount_Dynamic;
	int		gSpotLightCount_Dynamic;
	int		gIsLightingEnabled_Dynamic;
	float3	gCamPos;	float mPad1;
};

cbuffer cbPerSubset
{
	//Material
	N_Material_Basic	gMaterial;
	int				gIsDiffuseMapValid;
	int				gIsNormalMapValid;
	int				gIsSpecularMapValid;
	int				mPad2;
};

cbuffer	cbRarely
{
	//！！！！Static Light！！！！
	N_DirectionalLight gDirectionalLight_Static[50];
	N_PointLight	 gPointLight_Static[50];
	N_SpotLight	gSpotLight_Static[50];
	int		gDirectionalLightCount_Static;
	int		gPointLightCount_Static;
	int		gSpotLightCount_Static;
	int		gIsLightingEnabled_Static;
};


cbuffer cbSolid3D
{
	float4x4	gProjMatrix_Line3D;
	float4x4	gViewMatrix_Line3D;
};


cbuffer cbAtmosphere
{
	float3	gFogColor3;				
	int		gFogEnabled;
	float	gFogNear;
	float	gFogFar;
	float		gPad1;
	float		gPad2;
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
		
		//now specular	----reflect () : input an incoming light,and output an outgoing light
		float3 tmpV = reflect(UnitLightVec,Normal);
		
		//remember to normalize vectors to be "dotted"
		Vec_toCam = normalize(Vec_toCam);
		
		//to see if the specular light can be seen ... the SpecIntensity is the power of the cos factor
		float  SpecFactor = lightSpecIntensity * pow(max(dot(tmpV,Vec_toCam),0.0f),matSpecSmoothLevel);
		
		//final SpecularColor
		outSpecular = Attenuation * SpecFactor * float4(matSpecColor3 *lightSpecColor3,1.0f);
	}

}


void	SampleFromTexture(float2 TexCoord,out float3 outDiffColor3)
{
	//let's see if diffuse map is valid, and determine whether to use diffuse map
	outDiffColor3 = float3(0, 0, 0);
	if (gIsDiffuseMapValid)
	{
		outDiffColor3 = gDiffuseMap.Sample(sampler_ANISOTROPIC, TexCoord).xyz;
	}
	else
	{
		//invalid diffuse map, we should use pure color of basic material
		outDiffColor3 = gMaterial.mDiffuseColor;
	}
}





void	ComputeDirLightColor(N_DirectionalLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,
							out float4 outColor4)
{
	outColor4 = float4(0,0,0,1.0f);
	float4 outAmbient4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outDiffuse4	= float4(0.0f,0.0f,0.0f,0.0f);
	float4 outSpec4		= float4(0.0f,0.0f,0.0f,0.0f);


	//direction of the incoming light
	float3	LightVec = normalize(Light.mDirection);
	
	//Diffuse factor :: the Cos(theta)
	float 	diffuseCosFactor = Light.mDiffuseIntensity *dot(-LightVec,NormalW);
	
	//sample color from textures
	float3 diffuseColor3 = float3(0, 0, 0);
	SampleFromTexture(TexCoord,diffuseColor3);

	ComputeOutput_Amb_Diff_Spec(diffuseCosFactor,Light.mSpecularIntensity,gMaterial.mSpecularSmoothLevel,1.0f,
									gMaterial.mAmbientColor	,Light.mAmbientColor,
									diffuseColor3			,Light.mDiffuseColor,
									gMaterial.mSpecularColor,Light.mSpecularColor,
									Vec_toCam,LightVec,NormalW,
									outAmbient4,outDiffuse4,outSpec4);
	

	outColor4 = outAmbient4 + outDiffuse4 + outSpec4;
};




void	ComputePointLightColor(N_PointLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 thisPoint,
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

	//Diffuse factor ::  Cos(theta)
	float diffuseCosFactor =Light.mDiffuseIntensity * dot(-Unit_LightVec,NormalW);
	
	
	//sample color from textures
	float3 diffuseColor3 = float3(0, 0, 0);
	SampleFromTexture(TexCoord,diffuseColor3);
	
	
	//final ambient ... "*" for 2 vectors stand for a component - wise multiplication (components are multiplied respectively)
	outAmbient4 = float4(gMaterial.mAmbientColor*Light.mAmbientColor*diffuseColor3, 1.0f);
	
	ComputeOutput_Amb_Diff_Spec(diffuseCosFactor,Light.mSpecularIntensity,gMaterial.mSpecularSmoothLevel,Attenuation,
									gMaterial.mAmbientColor	,Light.mAmbientColor,
									diffuseColor3			,Light.mDiffuseColor,
									gMaterial.mSpecularColor,Light.mSpecularColor,
									Vec_toCam,Unit_LightVec,NormalW,
									outAmbient4,outDiffuse4,outSpec4);

	outColor4 = outAmbient4 + outDiffuse4 + outSpec4;
};





void	ComputeSpotLightColor(N_SpotLight Light,float3 NormalW,float2 TexCoord,float3 Vec_toCam,float3 thisPoint,
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
	if((disFromLight > Light.mLightingRange)||(Cos_Theta < cos(Light.mLightingAngle)))
	{
		outColor4 = float4(0,0,0,1.0f);
		return;
	}

	//Diffuse factor ::  Cos(theta)
	float diffuseCosFactor = Light.mDiffuseIntensity *dot(-Unit_LightVec,NormalW);
	
	
	//sample color from textures
	float3 diffuseColor3 = float3(0, 0, 0);
	SampleFromTexture(TexCoord,diffuseColor3);

	ComputeOutput_Amb_Diff_Spec(diffuseCosFactor,Light.mSpecularIntensity,gMaterial.mSpecularSmoothLevel,Attenuation,
									gMaterial.mAmbientColor	,Light.mAmbientColor,
									diffuseColor3			,Light.mDiffuseColor,
									gMaterial.mSpecularColor,Light.mSpecularColor,
									Vec_toCam,Unit_LightVec,NormalW,
									outAmbient4,outDiffuse4,outSpec4);

									
	outColor4 = outAmbient4 + outDiffuse4 + outSpec4;
};
