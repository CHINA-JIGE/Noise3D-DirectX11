
/**********************************************************************

							File:Input Struct.fx
			input struct used by main.fx by VS/PS/xxx main shader functions

**************************************************************************/


/**********************************************************************
			SHADER	INPUT/OUTPUT   STRUCTURE    DEFINITION
**************************************************************************/
struct VS_INPUT_DRAW_MESH
{
	float3 posL : POSITION;
	float4 color : COLOR;
	float3 normalL :NORMAL;
	float2 texcoord: TEXCOORD;
	float3 tangentL : TANGENT;
};

//for phong shading/pixel lighting
struct VS_OUTPUT_DRAW_MESH_PHONG
{
	float4 posH : SV_POSITION;
	float4 color : COLOR;
	float3 posW: POSITION;
	float3 normalW :NORMAL;
	float2 texcoord:TEXCOORD;
	float3 tangentW :TANGENT;
};

//for gouraud shading/vertex lighting
struct VS_OUTPUT_DRAW_MESH_GOURAUD
{
	float4 posH : SV_POSITION;
	float4 ambient : COLOR0;
	float4 diffuse : COLOR1;
	float4 specular : COLOR2;
	float3 posW: POSITION;
	float3 normalW :NORMAL;
	float2 texcoord:TEXCOORD;
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

struct VS_OUTPUT_QWERTY
{
	//for qwerty 3d
	float4 posH : SV_POSITION;
	float3 posV : POSITION;
	float2 texcoord:TEXCOORD;
};

//--GS--
struct GS_INPUT_MINIMIZED
{
	float4 pos : SV_POSITION;
};

struct GS_OUTPUT_MINIMIZED
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

#define NOISE_LIGHT_TYPE_ID_DYNAMIC_DIR_LIGHT 0
#define NOISE_LIGHT_TYPE_ID_DYNAMIC_POINT_LIGHT 1
#define NOISE_LIGHT_TYPE_ID_DYNAMIC_SPOT_LIGHT 2
//there used to be static lights(2018.1.14)

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
	//there used to be int/BOOL variables for switching on/off 
	//mapping techniques/special effects
};

cbuffer	cbRarely
{
	//used to be "static" lights(2018.1.14)
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

/*********************************************************
								POST PROCESSING
**********************************************************/
//previous render target, use as post processing's shader input
Texture2D gPreviousRenderTarget;