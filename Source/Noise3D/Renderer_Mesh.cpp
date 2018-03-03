
/***********************************************************************

							Desc:  Noise Mesh Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IRenderModuleForMesh::IRenderModuleForMesh()
{

}

IRenderModuleForMesh::~IRenderModuleForMesh()
{
	ReleaseCOM(m_pFX_Tech_DrawMesh);
}

void	IRenderModuleForMesh::RenderMeshes()
{
	ICamera* const tmp_pCamera = GetScene()->GetCamera();

	mFunction_RenderMeshInList_UpdateRarely();

	mFunction_RenderMeshInList_UpdatePerFrame();

	m_pRefRI->UpdateCameraMatrix(tmp_pCamera);

	//for every mesh
	for (UINT i = 0; i<mRenderList_Mesh.size(); i++)
	{
		IMesh* const pMesh = mRenderList_Mesh.at(i);

		mFunction_RenderMeshInList_UpdatePerObject(pMesh);

		//IA/OM settings
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::DEFAULT, pMesh->m_pVB_Gpu, pMesh->m_pIB_Gpu, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pRefRI->SetRasterState(pMesh->GetFillMode(), pMesh->GetCullMode());
		m_pRefRI->SetBlendState(pMesh->GetBlendMode());
		m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, NOISE_SAMPLERMODE::LINEAR);
		m_pRefRI->SetDepthStencilState(true);
		m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);


		//every mesh subset(one for each material)
		UINT meshSubsetCount = pMesh->mSubsetInfoList.size();
		for (UINT j = 0;j < meshSubsetCount;j++)
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
			ID3DX11EffectPass* pPass = mFunction_RenderMeshInList_UpdatePerSubset(pMesh,j);
			pPass->Apply(0, g_pImmediateContext);
			g_pImmediateContext->DrawIndexed(currSubsetIndicesCount, currSubsetStartIndex, 0);
			
		}
	}
}

void IRenderModuleForMesh::AddToRenderQueue(IMesh* obj)
{
	mRenderList_Mesh.push_back(obj);
}




/***********************************************************
									PROTECTED
************************************************************/

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
	ILightManager* tmpLightMgr = GetScene()->GetLightMgr();

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

ID3DX11EffectPass*		IRenderModuleForMesh::mFunction_RenderMeshInList_UpdatePerSubset(IMesh* const pMesh,UINT subsetID)
{
	//we dont accept invalid material ,but accept invalid texture
	IScene* pScene = GetScene();
	ITextureManager*		pTexMgr = pScene->GetTextureMgr();
	IMaterialManager*		pMatMgr = pScene->GetMaterialMgr();
	IMeshManager*			pMeshMgr = pScene->GetMeshMgr();

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

	ITexture* pDiffMap = pTexMgr->GetTexture(tmpMat.diffuseMapName);
	ITexture* pNormalMap = pTexMgr->GetTexture(tmpMat.normalMapName);
	ITexture* pSpecMap = pTexMgr->GetTexture(tmpMat.specularMapName);
	ITexture* pEnvMap = pTexMgr->GetTexture(tmpMat.environmentMapName);
	bool isDiffuseMapValid = false;
	bool	isNormalMapValid = false;
	bool isSpecularMapValid = false;
	bool isEnvMapValid = false;

	//first validate if ID is valid (within range / valid ID) valid== return original texID
	if(pDiffMap)			isDiffuseMapValid = pDiffMap->IsTextureType(NOISE_TEXTURE_TYPE_COMMON);
						else	isDiffuseMapValid = false;

	if (pNormalMap)	isNormalMapValid = pNormalMap->IsTextureType(NOISE_TEXTURE_TYPE_COMMON);
						else	isNormalMapValid = false;

	if (pSpecMap)		isSpecularMapValid = pSpecMap->IsTextureType(NOISE_TEXTURE_TYPE_COMMON);
						else	isSpecularMapValid = false;

	if (pEnvMap)		isEnvMapValid = pEnvMap->IsTextureType(NOISE_TEXTURE_TYPE_CUBEMAP);
						else	isEnvMapValid = false;

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

void		IRenderModuleForMesh::mFunction_RenderMeshInList_UpdatePerObject(IMesh* const pMesh)
{
	//update world/worldInv matrix
	NMATRIX worldMat,worldInvTransposeMat;
	pMesh->GetWorldMatrix(worldMat, worldInvTransposeMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD, worldMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD_INV_TRANSPOSE, worldInvTransposeMat);
};

