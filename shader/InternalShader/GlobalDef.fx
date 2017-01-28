
/**********************************************************************

			File:Input Struct.fx
			input struct used by main.fx by VS/PS/xxx main shader functions

**************************************************************************/


/**********************************************************************
					INPUT   STRUCTURE    DEFINITION
**************************************************************************/
struct VS_INPUT_DEFAULT
{
	float3 posL : POSITION;
	float4 color : COLOR;
	float3 normalL :NORMAL;
	float2 texcoord: TEXCOORD;
	float3 tangentL : TANGENT;
};

struct VS_OUTPUT_DEFAULT
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
	float3 posW: POSITION;
	float3 normalW :NORMAL;
	float2 texcoord:TEXCOORD;
	float3 tangentW :TANGENT;
};

struct VS_INPUT_SIMPLE
{
	float3 posL:POSITION;
	float4 color:COLOR;
	float2 texcoord:TEXCOORD;
};

struct VS_OUTPUT_SIMPLE
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord:TEXCOORD;
};


struct GS_INPUT_MINIZED
{
	float4 pos : SV_POSITION;
};

struct GS_OUTPUT_MINIZED
{
	float4  pos : POSITION;
};

/**********************************************************************
					STRUCTURE DEFINITION
**************************************************************************/

struct N_DirectionalLight
{
	 float3 mAmbientColor;		float		mSpecularIntensity;
	 float3 mDiffuseColor;			float		mDiffuseIntensity;
	 float3 mSpecularColor;		float		mPad2;
	 float3 mDirection;				float		mPad3;
};

struct N_PointLight
{
	float3 mAmbientColor;			float mSpecularIntensity;
	float3 mDiffuseColor;				float mDiffuseIntensity;
	float3 mSpecularColor;			float mAttenuationFactor;
	float3 mPosition;						float mLightingRange;
};

struct N_SpotLight
{
	float3 mAmbientColor;			float mSpecularIntensity;
	float3 mDiffuseColor;			float mDiffuseIntensity;
	float3 mSpecularColor;			float mAttenuationFactor;
	float3 mLitAt;					float mLightingAngle;
	float3 mPosition;				float mLightingRange;
};

struct N_Material_Basic
{
	float3	mAmbientColor;		int		mSpecularSmoothLevel;
	float3	mDiffuseColor;		float	mNormalMapBumpIntensity;
	float3	mSpecularColor;		float	mEnvironmentMapTransparency;
};




/**********************************************************************
					GLOBAL CONSTANT    BUFFER
**************************************************************************/
cbuffer cbPerObject
{
	float4x4	gWorldMatrix;
	float4x4	gWorldInvTransposeMatrix;
};

cbuffer cbPerFrame
{
	//————Dynamic Light————
	N_DirectionalLight gDirectionalLight_Dynamic[10];
	N_PointLight	 gPointLight_Dynamic[10];
	N_SpotLight		gSpotLight_Dynamic[10];
	int		gDirectionalLightCount_Dynamic;
	int		gPointLightCount_Dynamic;
	int		gSpotLightCount_Dynamic;
	int		gIsLightingEnabled_Dynamic;

};

cbuffer cbPerSubset
{
	//Material
	N_Material_Basic		gMaterial;
	int				gIsDiffuseMapValid;
	int				gIsNormalMapValid;
	int				gIsSpecularMapValid;
	int				gIsEnvironmentMapVaild;
};

cbuffer	cbRarely
{
	//————Static Light————
	N_DirectionalLight gDirectionalLight_Static[50];
	N_PointLight	 gPointLight_Static[50];
	N_SpotLight		gSpotLight_Static[50];
	int		gDirectionalLightCount_Static;
	int		gPointLightCount_Static;
	int		gSpotLightCount_Static;
	int		gIsLightingEnabled_Static;
};


cbuffer cbCameraInfo
{
	float4x4	gProjMatrix;//to proj space
	float4x4	gViewMatrix;//to view space
	float4x4	gInvProjMatrix;
	float4x4	gInvViewMatrix;
	float3		gCamPos;	float mPad1;
};


SamplerState samplerDefault
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};
