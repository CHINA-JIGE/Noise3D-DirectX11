
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
	for (int i = 0; i < NOISE_SHADER_VAR_MATRIX_ELEMENT_COUNT; ++i) ReleaseCOM(m_pFxMatrix[i]);
	for (int i = 0; i < NOISE_SHADER_VAR_GENERAL_ELEMENT_COUNT; ++i) ReleaseCOM(m_pFxVar[i]);
	for (int i = 0; i < NOISE_SHADER_VAR_SCALAR_ELEMENT_COUNT; ++i) ReleaseCOM(m_pFxScalar[i]);
	for (int i = 0; i < NOISE_SHADER_VAR_VECTOR_ELEMENT_COUNT; ++i) ReleaseCOM(m_pFxVector[i]);
	for (int i = 0; i < NOISE_SHADER_VAR_SAMPLER_ELEMENT_COUNT; ++i) ReleaseCOM(m_pFxSampler[i]);
	for (int i = 0; i < NOISE_SHADER_VAR_TEXTURE_ELEMENT_COUNT; ++i) ReleaseCOM(m_pFxTexture[i]);

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
	m_pSingleton = new IShaderVariableManager;

	//1. initialization of the singleton, bind effect variables to shader variables
	//2. remember to maintain the 'order of enum definition' and 'order of var binding' consistent
#define BIND_SHADER_VAR_MATRIX(cppVarName,shaderVarName) m_pSingleton->m_pFxMatrix[NOISE_SHADER_VAR_MATRIX::cppVarName] =  g_pFX->GetVariableByName(shaderVarName)->AsMatrix()
	BIND_SHADER_VAR_MATRIX(WORLD, "gWorldMatrix");
	BIND_SHADER_VAR_MATRIX(WORLD_INV_TRANSPOSE, "gWorldInvTransposeMatrix");
	BIND_SHADER_VAR_MATRIX(PROJECTION, "gProjMatrix");
	BIND_SHADER_VAR_MATRIX(VIEW, "gViewMatrix");
	BIND_SHADER_VAR_MATRIX(VIEW_INV, "gViewInvMatrix");

#define BIND_SHADER_VAR_GENERAL(cppVarName,shaderVarName) m_pSingleton->m_pFxVar[NOISE_SHADER_VAR_GENERAL::cppVarName] = g_pFX->GetVariableByName(shaderVarName)
	
	BIND_SHADER_VAR_GENERAL(DYNAMIC_DIRLIGHT, "gDirectionalLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(DYNAMIC_POINTLIGHT, "gPointLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(DYNAMIC_SPOTLIGHT, "gSpotLight_Dynamic");
	BIND_SHADER_VAR_GENERAL(MATERIAL_BASIC, "gMaterial");

#define BIND_SHADER_VAR_SCALAR(cppVarName,shaderVarName) m_pSingleton->m_pFxScalar[NOISE_SHADER_VAR_SCALAR::cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsScalar()

	BIND_SHADER_VAR_SCALAR(DYNAMIC_LIGHT_ENABLED, "gIsLightingEnabled_Dynamic");
	BIND_SHADER_VAR_SCALAR(DYNAMIC_DIRLIGHT_COUNT, "gDirectionalLightCount_Dynamic");
	BIND_SHADER_VAR_SCALAR(DYNAMIC_POINTLIGHT_COUNT, "gPointLightCount_Dynamic");
	BIND_SHADER_VAR_SCALAR(DYNAMIC_SPOTLIGHT_COUNT, "gSpotLightCount_Dynamic");
	BIND_SHADER_VAR_SCALAR(FOG_ENABLED, "gFogEnabled");
	BIND_SHADER_VAR_SCALAR(FOG_NEAR, "gFogNear");
	BIND_SHADER_VAR_SCALAR(FOG_FAR, "gFogFar");
	BIND_SHADER_VAR_SCALAR(SKYBOX_WIDTH, "gSkyBoxWidth");
	BIND_SHADER_VAR_SCALAR(SKYBOX_HEIGHT, "gSkyBoxHeight");
	BIND_SHADER_VAR_SCALAR(SKYBOX_DEPTH, "gSkyBoxDepth");

#define BIND_SHADER_VAR_VECTOR(cppVarName,shaderVarName) m_pSingleton->m_pFxVector[cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsVector()

	BIND_SHADER_VAR_VECTOR(CAMERA_POS3, "gCamPos");
	BIND_SHADER_VAR_VECTOR(FOG_COLOR3, "gFogColor3");
	BIND_SHADER_VAR_VECTOR(TEXT_COLOR4, "g2D_TextColor");
	BIND_SHADER_VAR_VECTOR(TEXT_GLOW_COLOR4, "g2D_TextGlowColor");
	BIND_SHADER_VAR_VECTOR(PICKING_RAY_NORMALIZED_DIR_XY, "gPickingRayNormalizedDirXY");

#define BIND_SHADER_VAR_SAMPLER(cppVarName,shaderVarName) m_pSingleton->m_pFxSampler[cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsSampler()

	BIND_SHADER_VAR_SAMPLER(DEFAULT_SAMPLER, "samplerDefault");
	BIND_SHADER_VAR_SAMPLER(DRAW_2D_SAMPLER, "samplerDraw2D");

#define BIND_SHADER_VAR_TEXTURE(cppVarName,shaderVarName) m_pSingleton->m_pFxTexture[cppVarName] = g_pFX->GetVariableByName(shaderVarName)->AsShaderResource()

	BIND_SHADER_VAR_TEXTURE(DIFFUSE_MAP, "gDiffuseMap");
	BIND_SHADER_VAR_TEXTURE(NORMAL_MAP, "gNormalMap");
	BIND_SHADER_VAR_TEXTURE(SPECULAR_MAP, "gSpecularMap");
	BIND_SHADER_VAR_TEXTURE(CUBE_MAP, "gCubeMap");//environment mapping
	BIND_SHADER_VAR_TEXTURE(COLOR_MAP_2D, "gColorMap2D");//for 2d texturing
	BIND_SHADER_VAR_TEXTURE(POST_PROCESS_PREV_RT, "gPreviousRenderTarget");//RenderTarget use as next pass's shader input
	

	return m_pSingleton;
}

void IShaderVariableManager::SetVar(const char * var, void * pVal, int size)
{
	g_pFX->GetVariableByName(var)->SetRawValue(pVal, 0, size);
}

void IShaderVariableManager::SetMatrix(NOISE_SHADER_VAR_MATRIX var, const NMATRIX & data)
{
	//Effect::SetMatrix will automatically transpose the data
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

void IShaderVariableManager::SetDynamicDirLight(int index, const N_DirLightDesc & dynamicLightDesc)
{
	uint32_t byteOffset = sizeof(dynamicLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_DIRLIGHT]->SetRawValue(&dynamicLightDesc, byteOffset, sizeof(dynamicLightDesc));
	//m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_DIRLIGHT]->GetElement(index)->SetRawValue(&dynamicLightDesc, 0, sizeof(dynamicLightDesc));
}

void IShaderVariableManager::SetDynamicPointLight(int index, const N_PointLightDesc & dynamicLightDesc)
{
	uint32_t byteOffset = sizeof(dynamicLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_POINTLIGHT]->SetRawValue(&dynamicLightDesc, byteOffset, sizeof(dynamicLightDesc));
	//m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_POINTLIGHT]->GetElement(index)->SetRawValue(&dynamicLightDesc, 0, sizeof(dynamicLightDesc));
}

void IShaderVariableManager::SetDynamicSpotLight(int index, const N_SpotLightDesc & dynamicLightDesc)
{
	uint32_t byteOffset = sizeof(dynamicLightDesc) * index;
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_SPOTLIGHT]->SetRawValue(&dynamicLightDesc, byteOffset, sizeof(dynamicLightDesc));
	//m_pFxVar[NOISE_SHADER_VAR_GENERAL::DYNAMIC_SPOTLIGHT]->GetElement(index)->SetRawValue(&dynamicLightDesc, 0, sizeof(dynamicLightDesc));
}

void IShaderVariableManager::SetMaterial(N_BasicMaterialDesc  mat)
{
	m_pFxVar[NOISE_SHADER_VAR_GENERAL::MATERIAL_BASIC]->SetRawValue(&mat, 0, sizeof(mat));
}
