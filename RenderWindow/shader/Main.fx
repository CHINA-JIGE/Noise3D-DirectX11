/**********************************************************************

	File: Main.fx
	Author: Jige

**************************************************************************/

#include "input_struct.fx"
#include "shading3D.fx"
#include "shading2D.fx"


//---------------------------------Default Draw--------------------------------------
VS_OUTPUT_DEFAULT VS_DefaultDraw(VS_INPUT_DEFAULT input )
{
		float3x3 gWorldMatrixWithoutTranslation;
		gWorldMatrixWithoutTranslation[0] = gWorldMatrix[0].xyz;
		gWorldMatrixWithoutTranslation[1] = gWorldMatrix[1].xyz;
		gWorldMatrixWithoutTranslation[2] = gWorldMatrix[2].xyz;
		
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
		//transform tangent to help implement XYZ to TBN
		output.tangentW = mul(input.tangentL,gWorldMatrixWithoutTranslation);
		//texture coordinate
		output.texcoord = input.texcoord;

    	return output;
}

float4 PS_DefaultDraw(VS_OUTPUT_DEFAULT input ) : SV_Target //渲染到system需要的texture，就是 back buffer ，所以叫SV_Target
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
	
	
	//compute fog effect
	//skip pixel if it is totally fogged
	float Dist_CurrPointToCam = length(Vec_ToCam);
	if(gFogEnabled==1 && Dist_CurrPointToCam>gFogFar)
	{
		return float4(gFogColor3,1.0f);
	}

	
	//compute DYNAMIC LIGHT
	int i = 0;
	if(gIsLightingEnabled_Dynamic)
	{
		for(i=0;i<gDirectionalLightCount_Dynamic;i++)
		{
			ComputeDirLightColor(gDirectionalLight_Dynamic[i],input.normalW,input.texcoord,Vec_ToCam,input.tangentW,tmpColor4);
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gPointLightCount_Dynamic;i++)
		{
			ComputePointLightColor(gPointLight_Dynamic[i],input.normalW,input.texcoord,Vec_ToCam,input.posW,input.tangentW,tmpColor4);
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gSpotLightCount_Dynamic;i++)
		{

			ComputeSpotLightColor(gSpotLight_Dynamic[i],input.normalW,input.texcoord,Vec_ToCam,input.posW,input.tangentW,tmpColor4);
			finalColor4 += tmpColor4;
		} 
	}

	//compute STATIC LIGHT
	if(gIsLightingEnabled_Static)
	{
		for(i=0;i<gDirectionalLightCount_Static;i++)
		{
			ComputeDirLightColor(gDirectionalLight_Static[i],input.normalW,input.texcoord,Vec_ToCam,input.tangentW,tmpColor4);
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gPointLightCount_Static;i++)
		{
			ComputePointLightColor(gPointLight_Static[i],input.normalW,input.texcoord,Vec_ToCam,input.posW,input.tangentW,tmpColor4);
			finalColor4 += tmpColor4;
		}
		for(i=0;i<gSpotLightCount_Static;i++)
		{
			ComputeSpotLightColor(gSpotLight_Static[i],input.normalW,input.texcoord,Vec_ToCam,input.posW,input.tangentW,tmpColor4);
			finalColor4 += tmpColor4;
		} 
	}

	
	
	

	//at last compute fog  ( point farther than gFogFar has been skipped
	if(gFogEnabled)
	{
		float fogInterpolationFactor = max(0,(Dist_CurrPointToCam - gFogNear)/(gFogFar-gFogNear));
		finalColor4 = lerp(finalColor4,float4(gFogColor3,1.0f),fogInterpolationFactor);
	}

    return finalColor4;
}




//-----------------------------------Draw 3D Lines/Points------------------------------
VS_OUTPUT_SIMPLE VS_Solid3D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = mul(mul(float4(input.posL, 1.0f),gViewMatrix), gProjMatrix);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

float4 PS_Solid3D(VS_OUTPUT_SIMPLE input) : SV_Target
{
	return input.color;
}



//----------------------------------Draw 2D Lines/Points/Triangles----------------------------------
VS_OUTPUT_SIMPLE VS_Solid2D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = float4(input.posL,1.0f);
	output.color = input.color;
	return output;
}

float4 PS_Solid2D(VS_OUTPUT_SIMPLE input) : SV_Target
{
	return input.color;
}



//----------------------------------Draw Textured 2D Triangles----------------------------------
VS_OUTPUT_SIMPLE VS_Textured2D(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_SIMPLE output = (VS_OUTPUT_SIMPLE)0;
	output.posH = float4(input.posL,1.0f);//mul(mul(float4(input.posL, 1.0f),gViewMatrix), gProjMatrix);
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

float4 PS_Textured2D(VS_OUTPUT_SIMPLE input) : SV_Target
{
	float4 sampledColor = g2D_DiffuseMap.Sample(sampler2D_ANISOTROPIC, input.texcoord);
	float4 outputColor = float4(sampledColor.xyz,sampledColor.w * input.color.w);
	return outputColor;
}



//--------------------------------------------Draw Sky------------------------------------------
VS_OUTPUT_DEFAULT VS_DrawSky(VS_INPUT_SIMPLE input)
{
	VS_OUTPUT_DEFAULT output = (VS_OUTPUT_DEFAULT)0;
	
	float3x3 viewMatrixWithoutTranslation;
	viewMatrixWithoutTranslation[0]=gViewMatrix[0].xyz;
	viewMatrixWithoutTranslation[1]=gViewMatrix[1].xyz;
	viewMatrixWithoutTranslation[2]=gViewMatrix[2].xyz;
	
	//so the sky will always be in the same relative position with camera  (ww means the triangles lie in  infinitely far from original point)
	//output.posH = float4(mul(float4(mul(input.posL,viewMatrixWithoutTranslation),1.0f), gProjMatrix).xy,1.0f,1.0f);
	output.posH = mul(float4(mul(input.posL,viewMatrixWithoutTranslation),1.0f), gProjMatrix).xyww;
	output.posW = input.posL;
	output.color = input.color;
	output.texcoord = input.texcoord;
	return output;
}

float4 PS_DrawSky(VS_OUTPUT_DEFAULT input) : SV_Target
{
	float4 outputColor = input.color;
	
	//both skybox and skydome are invalid
	if((!gIsSkyDomeValid)&&(!gIsSkyBoxValid))
	{
		return outputColor;
	}
	if(gIsSkyDomeValid)
	{
		outputColor = gDiffuseMap.Sample(sampler2D_ANISOTROPIC, input.texcoord);
		return outputColor;
	}
	if(gIsSkyBoxValid)
	{
		//what we need to intersect sky box is a world-space Vector , but we should map the irregular skybox to a standard normalized box
		outputColor = gCubeMap.Sample(sampler2D_ANISOTROPIC,input.posW * float3(1/gSkyBoxWidth,1/gSkyBoxHeight,1/gSkyBoxDepth));
		return outputColor;
	}
	
	return outputColor;
}





//---------------------------------------------------------------------------------------

technique11 DefaultDraw
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0,VS_DefaultDraw()));
		SetPixelShader(CompileShader(ps_5_0,PS_DefaultDraw()));
	}
}

technique11 DrawSolid3D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid3D()));
		SetPixelShader(CompileShader(ps_5_0, PS_Solid3D()));
	}
}

technique11 DrawSolid2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Solid2D()));
		SetPixelShader(CompileShader(ps_5_0, PS_Solid2D()));
	}
}

technique11 DrawTextured2D
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Textured2D()));
		SetPixelShader(CompileShader(ps_5_0, PS_Textured2D()));
	}
}

technique11 DrawSky
{
	pass Pass0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_DrawSky()));
		SetPixelShader(CompileShader(ps_5_0, PS_DrawSky()));
		//SetDepthStencilState(LessEqualDSS, 0);
	}
}