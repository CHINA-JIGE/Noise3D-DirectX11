/**********************************************************************

	File: BasicShader.fx
	Author: Jige

**************************************************************************/


#include "Lighting.fx"

cbuffer cbPerObject
{
	float4x4	gWorldMatrix;
	float4x4	gWorldInvTransposeMatrix;
};

cbuffer cbPerFrame
{
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
	float4x4	gProjMatrix;

	//！！！！Static Light！！！！
	N_DirectionalLight gDirectionalLight_Static[50];
	N_PointLight	 gPointLight_Static[50];
	N_SpotLight	gSpotLight_Static[50];
	int		gDirectionalLightCount_Static;
	int		gPointLightCount_Static;
	int		gSpotLightCount_Static;
	int		gIsLightingEnabled_Static;
};

struct VS_INPUT
{
    float3 posL : POSITION;
    float4 color : COLOR;
	float3 normalL :NORMAL;
};

struct VS_OUTPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
	float3 posW: POSITION;
	float3 normalW :NORMAL;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS0( VS_INPUT input )
{
		//initialize
    	VS_OUTPUT output = (VS_OUTPUT)0;
		//the W transformation
		output.posW 	= mul(float4(input.posL,1.0f),gWorldMatrix).xyz;
		//the VP transformation
		output.posH 	= mul(mul(float4(output.posW,1.0f),gViewMatrix),gProjMatrix);
		//output the vertex color , this parameter will be used if the lighting system is off
    	output.color 	= input.color;
		//we need an normal vector in W space
		output.normalW 	= mul(float4(input.normalL,1.0f),gWorldInvTransposeMatrix).xyz;
		
    	return output;
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS0( VS_OUTPUT input ) : SV_Target
{
	//the output
	float4 finalColor = float4(0,0,0,0);
	
	//if the lighting system is off,then use vertex color
	if( (!gIsLightingEnabled_Dynamic)&& (!gIsLightingEnabled_Static) )
	{
		return input.color;
	}

	//interpolation can  'unnormalized' the unit  vector
	input.normalW = normalize(input.normalW);
	
	//vector ---- this point to Camera
	float3 Vec_ToCam = gCamPos - input.posW;
	
	//colors
	float4	outAmbient	= float4(0,0,0,0);		float4 tmpA= float4(0,0,0,0);
	float4	outDiffuse	= float4(0,0,0,0);		float4 tmpD= float4(0,0,0,0);
	float4	outSpecular	= float4(0,0,0,0);		float4 tmpS= float4(0,0,0,0);
	
	//compute DYNAMIC LIGHT
	int i = 0;
	if(gIsLightingEnabled_Dynamic)
	{
		for(i=0;i<gDirectionalLightCount_Dynamic;i++)
		{
			ComputeDirLightColor(gMaterial,gDirectionalLight_Dynamic[i],input.normalW,Vec_ToCam,tmpA,tmpD,tmpS);
			outAmbient+= tmpA;		outDiffuse+= tmpD;		outSpecular+= tmpS;
		}
		for(i=0;i<gPointLightCount_Dynamic;i++)
		{
			ComputePointLightColor(gMaterial,gPointLight_Dynamic[i],input.normalW,Vec_ToCam,input.posW,tmpA,tmpD,tmpS);
			outAmbient+= tmpA;		outDiffuse+= tmpD;		outSpecular+= tmpS;
		}
		for(i=0;i<gSpotLightCount_Dynamic;i++)
		{
			ComputeSpotLightColor(gMaterial,gSpotLight_Dynamic[i],input.normalW,Vec_ToCam,input.posW,tmpA,tmpD,tmpS);
			outAmbient+= tmpA;		outDiffuse+= tmpD;		outSpecular+= tmpS;
		} 
	}

	//compute STATIC LIGHT
	if(gIsLightingEnabled_Static)
	{
		for(i=0;i<gDirectionalLightCount_Static;i++)
		{
			ComputeDirLightColor(gMaterial,gDirectionalLight_Static[i],input.normalW,Vec_ToCam,tmpA,tmpD,tmpS);
			outAmbient+= tmpA;		outDiffuse+= tmpD;		outSpecular+= tmpS;
		}
		for(i=0;i<gPointLightCount_Static;i++)
		{
			ComputePointLightColor(gMaterial,gPointLight_Static[i],input.normalW,Vec_ToCam,input.posW,tmpA,tmpD,tmpS);
			outAmbient+= tmpA;		outDiffuse+= tmpD;		outSpecular+= tmpS;
		}
		for(i=0;i<gSpotLightCount_Static;i++)
		{
			ComputeSpotLightColor(gMaterial,gSpotLight_Static[i],input.normalW,Vec_ToCam,input.posW,tmpA,tmpD,tmpS);
			outAmbient+= tmpA;		outDiffuse+= tmpD;		outSpecular+= tmpS;
		} 
	}

	finalColor = outAmbient + outDiffuse + outSpecular;
	//clamp to [0,1]
	finalColor = saturate(finalColor);
    return finalColor;
}


//---------------------------------------------------------------------------------------
technique11 BasicTech
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_4_1,VS0()));
		SetPixelShader(CompileShader(ps_4_1,PS0()));
	}
}