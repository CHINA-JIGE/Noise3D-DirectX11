
/**********************************************************************

							File:Input Struct.fx
			input struct used by main.fx by VS/PS/xxx main shader functions

**************************************************************************/


/**********************************************************************
							INPUT   STRUCTURE    DEFINITION
**************************************************************************/
struct VS_INPUT_DRAW_MESH
{
	float3 posL : POSITION;
	float4 color : COLOR;
	float3 normalL :NORMAL;
	float2 texcoord: TEXCOORD;
	float3 tangentL : TANGENT;
};

struct VS_OUTPUT_DRAW_MESH
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
	float3  pos : POSITION0;
};

struct PS_OUTPUT_DRAW_MESH
{
	//SV stands for system value, internal & critical variable inside shader/directX
	float4 color : SV_Target;
};

struct PS_OUTPUT_SIMPLE
{
	float4 color : SV_Target;
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
	float3 mAmbientColor;		float mSpecularIntensity;
	float3 mDiffuseColor;			float mDiffuseIntensity;
	float3 mSpecularColor;		float mAttenuationFactor;
	float3 mLitAt;						float mLightingAngle;
	float3 mPosition;					float mLightingRange;
};

struct N_Material_Basic
{
	float3	mAmbientColor;		int	mSpecularSmoothLevel;
	float3	mDiffuseColor;		float	mNormalMapBumpIntensity;
	float3	mSpecularColor;		float	mEnvironmentMapTransparency;
	float		mTransparency;		int	pad1; int pad2; int pad3;
};


/**********************************************************************
					GLOBAL CONSTANT    BUFFER
**************************************************************************/

Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gSpecularMap;
TextureCube gCubeMap;

cbuffer cbPerObject
{
	float4x4	gWorldMatrix;
	float4x4	gWorldInvTransposeMatrix;
};

cbuffer cbCameraInfo
{
	float4x4	gProjMatrix;//to proj space
	float4x4	gViewMatrix;//to view space
	float4x4	gViewInvMatrix;
	float3		gCamPos;	//float mPad1;
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
	/*int				gIsDiffuseMapValid;// should be designed as uniform var
	int				gIsNormalMapValid;
	int				gIsSpecularMapValid;
	int				gIsEnvironmentMapVaild;*/
};

cbuffer	cbRarely
{
	//---------Static Light--------
	N_DirectionalLight gDirectionalLight_Static[50];
	N_PointLight	 gPointLight_Static[50];
	N_SpotLight		gSpotLight_Static[50];
	int		gDirectionalLightCount_Static;
	int		gPointLightCount_Static;
	int		gSpotLightCount_Static;
	int		gIsLightingEnabled_Static;
};



cbuffer cbAtmosphere
{
	float3	gFogColor3;
	int		gFogEnabled;
	float		gFogNear;
	float		gFogFar;
	float		gSkyBoxWidth;
	float		gSkyBoxHeight;
	float		gSkyBoxDepth;
};

//---SAMPLER
SamplerState samplerDefault
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 2;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

SamplerState samplerDraw2D
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};


/*********************************************************
											2 D
**********************************************************/

cbuffer cbDrawText2D
{
	float4 	g2D_TextColor;
	float4	g2D_TextGlowColor;
}

Texture2D gColorMap2D;
