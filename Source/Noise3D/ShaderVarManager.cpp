
/***********************************************************************

                           cpp£ºShader Variable Manager

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IShaderVariableManager::IShaderVariableManager()
{

}

IShaderVariableManager::~IShaderVariableManager()
{
}


bool IShaderVariableManager::Init()
{
	if (g_pFX == nullptr)
	{
		ERROR_MSG("IShaderVariableManager : effect is not initialized!");
		return false;
	}

	//BIND SHADER Variables to cpp interfaces:
	//for example : 
	//
	//	m_pFxMatrix_World = g_pFX->GetVariableByName("gWorldMatrix")->AsMatrix();
	//
#define BIND_SHADER_VAR_MATRIX(cppVarName,shaderVarName) m_pFxMatrix_##cppVarName## =  g_pFX->GetVariableByName(shaderVarName)->AsMatrix();

	BIND_SHADER_VAR_MATRIX(World, "gWorldMatrix");
	BIND_SHADER_VAR_MATRIX(WorldInvTranspose, "gWorldInvTransposeMatrix");
	BIND_SHADER_VAR_MATRIX(Projection, "gProjMatrix");
	BIND_SHADER_VAR_MATRIX(View,"gViewMatrix");
	BIND_SHADER_VAR_MATRIX(ViewInv, "gViewInvMatrix");

#define BIND_SHADER_VAR_GENERAL(cppVarName,shaderVarName) m_pFxVar_##cppVarName## = g_pFX->GetVariableByName(shaderVarName)

	BIND_SHADER_VAR_GENERAL(DynamicDirLightArray, "gDirectionalLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(DynamicPointLightArray, "gPointLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(DynamicSpotLightArray, "gSpotLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(StaticDirLightArray, "gDirectionalLight_Static");
	BIND_SHADER_VAR_GENERAL(StaticPointLightArray, "gPointLight_Static");
	BIND_SHADER_VAR_GENERAL(StaticSpotLightArray, "gSpotLight_Static");

#define BIND_SHADER_VAR_SCALAR(cppVarName,shaderVarName,type) m_pFxScalar_##type##_##cppVarName## = g_pFX->GetVariableByName(shaderVarName)->AsScalar()

	BIND_SHADER_VAR_SCALAR(DynamicLightEnabled, "gIsLightingEnabled_Dynamic", int);
	BIND_SHADER_VAR_SCALAR(DynamicDirLightCount, "gDirectionalLightCount_Dynamic", int);
	BIND_SHADER_VAR_SCALAR(DynamicPointLightCount, "gPointLightCount_Dynamic", int);
	BIND_SHADER_VAR_SCALAR(DynamicSpotLightCount, "gSpotLightCount_Dynamic", int);
	BIND_SHADER_VAR_SCALAR(StaticLightEnabled, "gIsLightingEnabled_Static", int);
	BIND_SHADER_VAR_SCALAR(StaticDirLightCount, "gDirectionalLightCount_Static", int);
	BIND_SHADER_VAR_SCALAR(StaticPointLightCount, "gPointLightCount_Static", int);
	BIND_SHADER_VAR_SCALAR(StaticSpotLightCount, "gSpotLightCount_Static", int);
	BIND_SHADER_VAR_SCALAR(FogEnabled, "gFogEnabled", int);
	BIND_SHADER_VAR_SCALAR(FogNear, "gFogNear",float);
	BIND_SHADER_VAR_SCALAR(FogFar, "gFogFar", float);
	BIND_SHADER_VAR_SCALAR(SkyBoxWidth, "gSkyBoxWidth", float);
	BIND_SHADER_VAR_SCALAR(SkyBoxHeight, "gSkyBoxHeight", float);
	BIND_SHADER_VAR_SCALAR(SkyBoxDepth, "gSkyBoxDepth", float);

#define BIND_SHADER_VAR_VECTOR(cppVarName,shaderVarName,dimension)m_pFxVector##dimension##_##cppVarName## = g_pFX->GetVariableByName(shaderVarName)->AsVector()

	BIND_SHADER_VAR_VECTOR(CamPos, "gCamPos", 3);
	BIND_SHADER_VAR_VECTOR(FogColor, "gFogColor3", 3);
	BIND_SHADER_VAR_VECTOR(TextColor, "g2D_TextColor", 4);
	BIND_SHADER_VAR_VECTOR(TextGlowColor, "g2D_TextGlowColor", 4);

#define BIND_SHADER_VAR_SAMPLER(cppVarName,shaderVarName) m_pFxSampler_##cppVarName## = g_pFX->GetVariableByName(shaderVarName)->AsSampler()

	BIND_SHADER_VAR_SAMPLER(DefaultDraw, "samplerDefault");
	BIND_SHADER_VAR_SAMPLER(Draw2D, "samplerDraw2D");

#define BIND_SHADER_VAR_TEXTURE(cppVarName,shaderVarName) m_pFxTexture_##cppVarName## = g_pFX->GetVariableByName(shaderVarName)->AsShaderResource()

	BIND_SHADER_VAR_TEXTURE(DiffuseMap, "gDiffuseMap");
	BIND_SHADER_VAR_TEXTURE(NormalMap, "gNormalMap");
	BIND_SHADER_VAR_TEXTURE(SpecularMap, "gSpecularMap");
	BIND_SHADER_VAR_TEXTURE(CubeMap, "gCubeMap");//environment mapping
	BIND_SHADER_VAR_TEXTURE(ColorMap2D, "gColorMap2D");//for 2d texturing

	return true;
}

void Noise3D::IShaderVariableManager::SetVar(const char * var, void * pVal, int size)
{
	g_pFX->GetVariableByName(var)->SetRawValue(pVal, 0, size);
}
