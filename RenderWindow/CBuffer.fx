/**********************************************************************

	File: CBuffer
	Father:Main.fx

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
	N_Material	gMaterial;
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


cbuffer cbDrawLine3D
{
	float4x4	gProjMatrix_Line3D;
	float4x4	gViewMatrix_Line3D;
};