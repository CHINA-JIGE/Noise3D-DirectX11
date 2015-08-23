/**********************************************************************

	File: Main.fx
	Author: Jige

**************************************************************************/


#include "Lighting.fx"
#include "CBuffer.fx"
#include "Input Struct.fx"


//----------Default Draw---------------
VS_OUTPUT_DEFAULT VS0(VS_INPUT_DEFAULT input )
{
		//initialize
    	VS_OUTPUT_DEFAULT output = (VS_OUTPUT_DEFAULT)0;
		//the W transformation
		output.posW 	= mul(float4(input.posL,1.0f),gWorldMatrix).xyz;
		//the VP transformation
		output.posH 	= mul(mul(float4(output.posW,1.0f),gViewMatrix),gProjMatrix);
		//output the vertex color , this parameter will be used if the lighting system is off
    	output.color 	= input.color;
		//we need an normal vector in W space
		output.normalW 	= mul(float4(input.normalL,1.0f),gWorldInvTransposeMatrix).xyz;
		//texture coordinate
		output.texcoord = input.texcoord;

    	return output;
}

float4 PS0(VS_OUTPUT_DEFAULT input ) : SV_Target //渲染到system需要的texture，就是 back buffer ，所以叫SV_Target
{
	//the output
	float4 	finalColor4 	= float4(0,0,0,0);
	float4	tmpColor4	= float4(0,0,0,0);
	
	//if the lighting system and material are invalid ,then use vertex color
	if( (!gIsLightingEnabled_Dynamic)&& (!gIsLightingEnabled_Static))
	{
			return input.color;//vertex color
	}

	//interpolation can  'unnormalized' the unit  vector
	input.normalW = normalize(input.normalW);
	
	//vector ---- this point to Camera
	float3 Vec_ToCam = gCamPos - input.posW;
	
	
	//compute DYNAMIC LIGHT
	int i = 0;
	if(gIsLightingEnabled_Dynamic)
	{
		for(i=0;i<gDirectionalLightCount_Dynamic;i++)
		{
			ComputeDirLightColor(gMaterial,gDirectionalLight_Dynamic[i],input.normalW,Vec_ToCam,
								gIsDiffuseMapValid,gIsNormalMapValid,gIsSpecularMapValid,
								gDiffuseMap,gNormalMap,gSpecularMap,
								input.texcoord,tmpColor4);
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gPointLightCount_Dynamic;i++)
		{
			ComputePointLightColor(gMaterial,gPointLight_Dynamic[i],input.normalW,Vec_ToCam,input.posW,
								gIsDiffuseMapValid,gIsNormalMapValid,gIsSpecularMapValid, 
								gDiffuseMap,gNormalMap,gSpecularMap,
								input.texcoord,tmpColor4);
			
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gSpotLightCount_Dynamic;i++)
		{
			ComputeSpotLightColor(gMaterial,gSpotLight_Dynamic[i],input.normalW,Vec_ToCam,input.posW,
								gIsDiffuseMapValid,gIsNormalMapValid,gIsSpecularMapValid, 
								gDiffuseMap,gNormalMap,gSpecularMap,
								input.texcoord,tmpColor4);
			finalColor4 += tmpColor4;
		} 
	}

	//compute STATIC LIGHT
	if(gIsLightingEnabled_Static)
	{
		for(i=0;i<gDirectionalLightCount_Static;i++)
		{
			ComputeDirLightColor(gMaterial,gDirectionalLight_Static[i],input.normalW,Vec_ToCam,
								gIsDiffuseMapValid,gIsNormalMapValid,gIsSpecularMapValid, 
								gDiffuseMap,gNormalMap,gSpecularMap,
								input.texcoord,tmpColor4);
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gPointLightCount_Static;i++)
		{
			ComputePointLightColor(gMaterial,gPointLight_Static[i],input.normalW,Vec_ToCam,input.posW, 
								gIsDiffuseMapValid,gIsNormalMapValid,gIsSpecularMapValid, 
								gDiffuseMap,gNormalMap,gSpecularMap,
								input.texcoord,tmpColor4);
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gSpotLightCount_Static;i++)
		{
			ComputeSpotLightColor(gMaterial,gSpotLight_Static[i],input.normalW,Vec_ToCam,input.posW,
								gIsDiffuseMapValid,gIsNormalMapValid,gIsSpecularMapValid, 
								gDiffuseMap,gNormalMap,gSpecularMap,
								input.texcoord,tmpColor4);
			finalColor4 += tmpColor4;
		} 
	}


	//clamp to [0,1]
	finalColor4 = saturate(finalColor4);
    return finalColor4;
}



//----------Draw 3D Lines---------------
VS_OUTPUT_SIMPLE VS_Line3D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = mul(mul(float4(input.posL, 1.0f),gViewMatrix_Line3D), gProjMatrix_Line3D);
	output.color = input.color;

	return output;
}

float4 PS_Line3D(VS_OUTPUT_SIMPLE input) : SV_Target
{
	return input.color;
}



//---------------------------------------------------------------------------------------

technique11 DefaultDraw
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0,VS0()));
		SetPixelShader(CompileShader(ps_5_0,PS0()));
	}
}

technique11 DrawLine3D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Line3D()));
		SetPixelShader(CompileShader(ps_5_0, PS_Line3D()));
	}
}
