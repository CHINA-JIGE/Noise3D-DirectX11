
/***********************************************************************

							Desc:  Render Module for mesh
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;


IRenderModuleForMesh::IRenderModuleForMesh()
{

}

IRenderModuleForMesh::~IRenderModuleForMesh()
{
	ReleaseCOM(m_pFX_Tech_DrawMesh);
}


void IRenderModuleForMesh::AddToRenderQueue(Mesh* obj)
{
	mRenderList_Mesh.push_back(obj);
}




/***********************************************************
									PROTECTED
************************************************************/
void	IRenderModuleForMesh::RenderMeshes()
{
	Camera* const tmp_pCamera = GetScene()->GetCamera();
	m_pRefRI->UpdateCameraMatrix(tmp_pCamera);

	mFunction_RenderMeshInList_UpdateRarely();

	mFunction_RenderMeshInList_UpdatePerFrame();

	//for every mesh
	for (UINT i = 0; i<mRenderList_Mesh.size(); i++)
	{
		Mesh* const pMesh = mRenderList_Mesh.at(i);

		mFunction_RenderMeshInList_UpdatePerObject(pMesh);

		//IA/OM settings
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::DEFAULT, pMesh->m_pVB_Gpu, pMesh->m_pIB_Gpu, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pRefRI->SetRasterState(pMesh->GetFillMode(), pMesh->GetCullMode());
		m_pRefRI->SetBlendState(pMesh->GetBlendMode());
		m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, NOISE_SAMPLERMODE::LINEAR_WRAP);
		m_pRefRI->SetDepthStencilState(true);
		m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);


		//every mesh subset(one for each material)
		UINT meshSubsetCount = pMesh->mSubsetInfoList.size();
		for (UINT j = 0; j < meshSubsetCount; j++)
		{
			//subset info
			UINT currSubsetIndicesCount = pMesh->mSubsetInfoList.at(j).primitiveCount * 3;
			UINT currSubsetStartIndex = pMesh->mSubsetInfoList.at(j).startPrimitiveID * 3;

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//ATTENTION!! : each subset might have different materials, hences different
			//texture combinations. In consideration of efficiency, texture operations will be 
			//turned on/off by UNIFORM bool in shader(so each mapping will be turn on/off
			//in shader compilation stage).  N switches of multiple mapping will produce 2^N
			//passes for the c++ host program to choose. 
			//'passID' will be computed to choose appropriate pass.
			ID3DX11EffectPass* pPass = mFunction_RenderMeshInList_UpdatePerSubset(pMesh, j);
			pPass->Apply(0, g_pImmediateContext);
			g_pImmediateContext->DrawIndexed(currSubsetIndicesCount, currSubsetStartIndex, 0);

		}
	}
}

void IRenderModuleForMesh::ClearRenderList()
{
	mRenderList_Mesh.clear();
}

bool IRenderModuleForMesh::Initialize(IRenderInfrastructure * pRI, IShaderVariableManager * pShaderVarMgr)
{
	m_pRefRI = pRI;
	m_pRefShaderVarMgr = pShaderVarMgr;
	m_pFX_Tech_DrawMesh = g_pFX->GetTechniqueByName("DrawMesh");
	return true;
}

/***********************************************************
										PRIVATE
***********************************************************/

void		IRenderModuleForMesh::mFunction_RenderMeshInList_UpdateRarely()
{
	//From 2018.1.14 on, static lights are used to bake static lights(light maps)
	//and not sent to GPU for lighting
};

void		IRenderModuleForMesh::mFunction_RenderMeshInList_UpdatePerFrame()
{
	//-------Update Dynamic Light-------
	LightManager* tmpLightMgr = GetScene()->GetLightMgr();

	UINT dirLightCount = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_DIR);
	UINT pointLightCount = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_POINT);
	UINT spotLightCount = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_SPOT);

	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_LIGHT_ENABLED, tmpLightMgr->IsDynamicLightingEnabled());
	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_DIRLIGHT_COUNT, dirLightCount);
	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_POINTLIGHT_COUNT, pointLightCount);
	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_SPOTLIGHT_COUNT, spotLightCount);

	for (UINT i = 0; i<(dirLightCount); i++)
	{
		m_pRefShaderVarMgr->SetDynamicDirLight(i, tmpLightMgr->GetDirLightD(i)->GetDesc());
	}

	for (UINT i = 0; i<(pointLightCount); i++)
	{
		m_pRefShaderVarMgr->SetDynamicPointLight(i, tmpLightMgr->GetPointLightD(i)->GetDesc());
	}

	for (UINT i = 0; i<(spotLightCount); i++)
	{
		m_pRefShaderVarMgr->SetDynamicSpotLight(i, tmpLightMgr->GetSpotLightD(i)->GetDesc());
	}
};

ID3DX11EffectPass*		IRenderModuleForMesh::mFunction_RenderMeshInList_UpdatePerSubset(Mesh* const pMesh,UINT subsetID)
{
	//we dont accept invalid material ,but accept invalid texture
	IScene* pScene = GetScene();
	TextureManager*		pTexMgr = pScene->GetTextureMgr();
	MaterialManager*		pMatMgr = pScene->GetMaterialMgr();
	MeshManager*			pMeshMgr = pScene->GetMeshMgr();

	//Get Material ID by unique name
	N_UID	 currSubsetMatName = pMesh->mSubsetInfoList.at(subsetID).matName;
	bool  IsMatNameValid = pMatMgr->ValidateUID(currSubsetMatName);

	//if material ID == INVALID_MAT_ID , then we should use default mat defined in mat mgr
	//then we should check if its child textureS are valid too 
	N_MaterialDesc tmpMat;
	if (IsMatNameValid== false)
	{
		WARNING_MSG("IRenderer : material UID not valid !");
		pMatMgr->GetDefaultMaterial()->GetDesc(tmpMat);
	}
	else
	{
		pMatMgr->GetMaterial(currSubsetMatName)->GetDesc(tmpMat);
	}

	//update basic material info
	m_pRefShaderVarMgr->SetMaterial(tmpMat);

	//Validate textures
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;

	ITexture* pDiffMap = pTexMgr->GetTexture2D(tmpMat.diffuseMapName);
	ITexture* pNormalMap = pTexMgr->GetTexture2D(tmpMat.normalMapName);
	ITexture* pSpecMap = pTexMgr->GetTexture2D(tmpMat.specularMapName);
	TextureCubeMap* pEnvMap = pTexMgr->GetTextureCubeMap(tmpMat.environmentMapName);
	bool isDiffuseMapValid = (pDiffMap!=nullptr);
	bool	isNormalMapValid = (pNormalMap!=nullptr);
	bool isSpecularMapValid = (pSpecMap!=nullptr);
	bool isEnvMapValid = (pEnvMap!=nullptr);

	//update textures, bound corresponding ShaderResourceView to the pipeline
	//if tetxure is  valid ,then set diffuse map
	if (isDiffuseMapValid)
	{
		auto pSRV = m_pRefRI->GetTextureSRV(pDiffMap);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::DIFFUSE_MAP, pSRV);
	}

	//if texture is  valid ,then set normal map
	if (isNormalMapValid)
	{
		auto pSRV = m_pRefRI->GetTextureSRV(pNormalMap);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::NORMAL_MAP, pSRV);
	}

	//if texture is  valid ,then set specular map
	if (isSpecularMapValid)
	{
		auto pSRV = m_pRefRI->GetTextureSRV(pSpecMap);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::SPECULAR_MAP, pSRV);
	}

	//if texture is  valid ,then set environment map (cube map)
	if (isEnvMapValid)
	{
		auto pSRV = m_pRefRI->GetTextureSRV(pEnvMap);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::CUBE_MAP, pSRV);
	}

	//return ID3DX11EffectPass interface to choose appropriate shader
	//each bit for each switch, then bitwise-AND all the switches
	ID3DX11EffectPass* pPass = nullptr;

	//NOTE that: special reneder technique like NORMAL MAPPING can only be
	//implemented in TANGENT SPACE
	bool isPerVertexLighting = (pMesh->GetShadeMode() == NOISE_SHADEMODE_GOURAUD);
	if (isPerVertexLighting)
	{
		if (isDiffuseMapValid)
		{
			pPass = m_pFX_Tech_DrawMesh->GetPassByName("perVertex_enableDiffMap");
			return pPass;
		}
		else
		{
			pPass = m_pFX_Tech_DrawMesh->GetPassByName("perVertex_disableDiffMap");
			return pPass;
		}
	}

	//determine per-pixel drawing pass (2^switchCount shaders are caches)
	constexpr int perPixelRenderSwitchCount = 4;
	int renderEffectSwitches[perPixelRenderSwitchCount] =
	{ 
		isDiffuseMapValid, 
		isNormalMapValid,
		isSpecularMapValid,
		isEnvMapValid
	};

	uint32_t passID = 0;
	for (uint32_t switchID = 0; switchID < perPixelRenderSwitchCount; ++switchID)
	{
		passID |= (renderEffectSwitches[switchID] << switchID);
	}
	pPass = m_pFX_Tech_DrawMesh->GetPassByIndex(passID);//name "perPixel_xxx"
	return pPass;
};

void		IRenderModuleForMesh::mFunction_RenderMeshInList_UpdatePerObject(Mesh* const pMesh)
{
	//update world/worldInv matrix
	NMATRIX worldMat,worldInvTransposeMat;
	pMesh->GetWorldMatrix(worldMat, worldInvTransposeMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD, worldMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD_INV_TRANSPOSE, worldInvTransposeMat);
};

