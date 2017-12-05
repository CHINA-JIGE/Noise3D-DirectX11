
/***********************************************************************

                           cpp£ºShader Variable Manager

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IShaderVariableManager* IShaderVariableManager::m_pSingleton = nullptr;

IShaderVariableManager::IShaderVariableManager()
{

}

IShaderVariableManager::~IShaderVariableManager()
{
	for (int i = 0; i < NOISE_SHADER_VAR_MATRIX_ELEMENT_COUNT; ++i) m_pFxMatrix[i]->Release();
	for (int i = 0; i < NOISE_SHADER_VAR_GENERAL_ELEMENT_COUNT; ++i) m_pFxVar[i]->Release();
	for (int i = 0; i < NOISE_SHADER_VAR_SCALAR_ELEMENT_COUNT; ++i) m_pFxScalar[i]->Release();
	for (int i = 0; i < NOISE_SHADER_VAR_VECTOR_ELEMENT_COUNT; ++i) m_pFxVector[i]->Release();
	for (int i = 0; i < NOISE_SHADER_VAR_SAMPLER_ELEMENT_COUNT; ++i) m_pFxSampler[i]->Release();
	for (int i = 0; i < NOISE_SHADER_VAR_TEXTURE_ELEMENT_COUNT; ++i) m_pFxTexture[i]->Release();

}

IShaderVariableManager* IShaderVariableManager::GetSingleton()
{
	//return singleton if it has already been created
	if (m_pSingleton!=nullptr)return m_pSingleton;

	if (g_pFX == nullptr)
	{
		ERROR_MSG("IShaderVariableManager : shader FX is not initialized!");
		return false;
	}

	//BIND SHADER Variables to cpp interfaces:
	//for example : 
	//
	//	m_pFxMatrix_World = g_pFX->GetVariableByName("gWorldMatrix")->AsMatrix();
	//
#define BIND_SHADER_VAR_MATRIX(cppVarName,shaderVarName) m_pFxMatrix[NOISE_SHADER_VAR_MATRIX::cppVarName] =  g_pFX->GetVariableByName(shaderVarName)->AsMatrix();
	BIND_SHADER_VAR_MATRIX(WORLD, "gWorldMatrix");
	BIND_SHADER_VAR_MATRIX(WORLD_INV_TRANSPOSE, "gWorldInvTransposeMatrix");
	BIND_SHADER_VAR_MATRIX(PROJECTION, "gProjMatrix");
	BIND_SHADER_VAR_MATRIX(VIEW, "gViewMatrix");
	BIND_SHADER_VAR_MATRIX(VIEW_INV, "gViewInvMatrix");

#define BIND_SHADER_VAR_GENERAL(cppVarName,shaderVarName) m_pFxVar[NOISE_SHADER_VAR_GENERAL::cppVarName] = g_pFX->GetVariableByName(shaderVarName)

	BIND_SHADER_VAR_GENERAL(DYNAMIC_DIRLIGHT, "gDirectionalLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(DYNAMIC_POINTLIGHT, "gPointLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(DYNAMIC_SPOTLIGHT, "gSpotLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(STATIC_DIRLIGHT, "gDirectionalLight_Static");
	BIND_SHADER_VAR_GENERAL(STATIC_POINTLIGHT, "gPointLight_Static");
	BIND_SHADER_VAR_GENERAL(STATIC_SPOTLIGHT, "gSpotLight_Static");

#define BIND_SHADER_VAR_SCALAR(cppVarName,shaderVarName) m_pFxScalar[NOISE_SHADER_VAR_SCALAR::cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsScalar()

	BIND_SHADER_VAR_SCALAR(DYNAMIC_LIGHT_ENABLED, "gIsLightingEnabled_Dynamic");
	BIND_SHADER_VAR_SCALAR(DYNAMIC_DIRLIGHT_COUNT, "gDirectionalLightCount_Dynamic");
	BIND_SHADER_VAR_SCALAR(DYNAMIC_POINTLIGHT_COUNT, "gPointLightCount_Dynamic");
	BIND_SHADER_VAR_SCALAR(DYNAMIC_SPOTLIGHT_COUNT, "gSpotLightCount_Dynamic");
	BIND_SHADER_VAR_SCALAR(STATIC_LIGHT_ENABLED, "gIsLightingEnabled_Static");
	BIND_SHADER_VAR_SCALAR(STATIC_DIRLIGHT_COUNT, "gDirectionalLightCount_Static");
	BIND_SHADER_VAR_SCALAR(STATIC_POINTLIGHT_COUNT, "gPointLightCount_Static");
	BIND_SHADER_VAR_SCALAR(STATIC_SPOTLIGHT_COUNT, "gSpotLightCount_Static");
	BIND_SHADER_VAR_SCALAR(FOG_ENABLED, "gFogEnabled");
	BIND_SHADER_VAR_SCALAR(FOG_NEAR, "gFogNear");
	BIND_SHADER_VAR_SCALAR(FOG_FAR, "gFogFar");
	BIND_SHADER_VAR_SCALAR(SKYBOX_WIDTH, "gSkyBoxWidth");
	BIND_SHADER_VAR_SCALAR(SKYBOX_HEIGHT, "gSkyBoxHeight");
	BIND_SHADER_VAR_SCALAR(SKYBOX_DEPTH, "gSkyBoxDepth");

#define BIND_SHADER_VAR_VECTOR(cppVarName,shaderVarName)m_pFxVector[cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsVector()

	BIND_SHADER_VAR_VECTOR(CAMERA_POS3, "gCamPos");
	BIND_SHADER_VAR_VECTOR(FOG_COLOR3, "gFogColor3");
	BIND_SHADER_VAR_VECTOR(TEXT_COLOR4, "g2D_TextColor");
	BIND_SHADER_VAR_VECTOR(TEXT_GLOW_COLOR4, "g2D_TextGlowColor");
	BIND_SHADER_VAR_VECTOR(PICKING_RAY_NORMALIZED_DIR_XY, "gPickingRayNormalizedDirXY");

#define BIND_SHADER_VAR_SAMPLER(cppVarName,shaderVarName) m_pFxSampler[cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsSampler()

	BIND_SHADER_VAR_SAMPLER(DEFAULT, "samplerDefault");
	BIND_SHADER_VAR_SAMPLER(DRAW_2D, "samplerDraw2D");

#define BIND_SHADER_VAR_TEXTURE(cppVarName,shaderVarName) m_pFxTexture[cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsShaderResource()

	BIND_SHADER_VAR_TEXTURE(DIFFUSE_MAP, "gDiffuseMap");
	BIND_SHADER_VAR_TEXTURE(NORMAL_MAP, "gNormalMap");
	BIND_SHADER_VAR_TEXTURE(SPECULAR_MAP, "gSpecularMap");
	BIND_SHADER_VAR_TEXTURE(CUBE_MAP, "gCubeMap");//environment mapping
	BIND_SHADER_VAR_TEXTURE(COLOR_MAP_2D, "gColorMap2D");//for 2d texturing

	m_pSingleton = new IShaderVariableManager;
	return m_pSingleton;
}

void Noise3D::IShaderVariableManager::SetVar(const char * var, void * pVal, int size)
{
	g_pFX->GetVariableByName(var)->SetRawValue(pVal, 0, size);
}

void IShaderVariableManager::SetMatrix(NOISE_SHADER_VAR_MATRIX var, const NMATRIX & data)
{
	m_pFxMatrix[var]->SetMatrix((float*)&data);
}

void IShaderVariableManager::SetVector2(NOISE_SHADER_VAR_VECTOR var, const NVECTOR2 & data)
{
	//so the dimension of the vector is managed by Effects11???
	m_pFxVector[var]->SetFloatVector((float*)&data);
}

void IShaderVariableManager::SetVector3(NOISE_SHADER_VAR_VECTOR var, const NVECTOR3 & data)
{
	m_pFxVector[var]->SetFloatVector((float*)&data);
}

void IShaderVariableManager::SetVector4(NOISE_SHADER_VAR_VECTOR var, const NVECTOR4 & data)
{
	m_pFxVector[var]->SetFloatVector((float*)&data);
}

void IShaderVariableManager::SetFloat(NOISE_SHADER_VAR_SCALAR var, float val)
{
	m_pFxScalar[var]->SetFloat(val);
}

void IShaderVariableManager::SetInt(NOISE_SHADER_VAR_SCALAR var, int val)
{
	m_pFxScalar[var]->SetInt(val);
}

void IShaderVariableManager::SetSampler(NOISE_SHADER_VAR_SAMPLER var,int index, ID3D11SamplerState * pState)
{
	m_pFxSampler[var]->SetSampler(index, pState);
}

void IShaderVariableManager::SetTexture(NOISE_SHADER_VAR_TEXTURE var, ID3D11ShaderResourceView * pSRV)
{
	m_pFxTexture[var]->SetResource(pSRV);
}

void IShaderVariableManager::SetStaticDirLight(int index, const N_DirLightDesc & staticLightDesc)
{
	int byteOffset = sizeof(staticLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::STATIC_DIRLIGHT]->SetRawValue(&staticLightDesc, byteOffset, sizeof(staticLightDesc));
}

void IShaderVariableManager::SetStaticPointLight(int index, const N_PointLightDesc & staticLightDesc)
{
	int byteOffset = sizeof(staticLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::STATIC_POINTLIGHT]->SetRawValue(&staticLightDesc, byteOffset, sizeof(staticLightDesc));
}

void IShaderVariableManager::SetStaticSpotLight(int index, const N_SpotLightDesc & staticLightDesc)
{
	int byteOffset = sizeof(staticLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::STATIC_SPOTLIGHT]->SetRawValue(&staticLightDesc, byteOffset, sizeof(staticLightDesc));
}

void IShaderVariableManager::SetDynamicDirLight(int index, const N_DirLightDesc & dynamicLightDesc)
{
	int byteOffset = sizeof(dynamicLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_DIRLIGHT]->SetRawValue(&dynamicLightDesc, byteOffset, sizeof(dynamicLightDesc));
}

void IShaderVariableManager::SetDynamicPointLight(int index, const N_PointLightDesc & dynamicLightDesc)
{
	int byteOffset = sizeof(dynamicLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_POINTLIGHT]->SetRawValue(&dynamicLightDesc, byteOffset, sizeof(dynamicLightDesc));
}

void IShaderVariableManager::SetDynamicSpotLight(int index, const N_SpotLightDesc & dynamicLightDesc)
{
	int byteOffset = sizeof(dynamicLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_SPOTLIGHT]->SetRawValue(&dynamicLightDesc, byteOffset, sizeof(dynamicLightDesc));
}
