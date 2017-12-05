
/***********************************************************************

							Desc:  Noise Mesh Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void	IRenderer::RenderMeshes()
{
	ICamera* const tmp_pCamera = GetScene()->GetCamera();

	//更新ConstantBuffer: Only update when modified(cbRarely)
	mFunction_RenderMeshInList_UpdateRarely();

	//Update ConstantBuffer: Once Per Frame (cbPerFrame)
	mFunction_RenderMeshInList_UpdatePerFrame(tmp_pCamera);

	//Update ConstantBuffer : Proj / View Matrix (this function could be used elsewhere)
	mFunction_CameraMatrix_Update(tmp_pCamera);


#pragma region Render Mesh
	//for every mesh
	for (UINT i = 0; i<(m_pRenderList_Mesh->size()); i++)
	{
		//取出渲染列表中的mesh指针
		IMesh* const pMesh = m_pRenderList_Mesh->at(i);

		//更新ConstantBuffer:每物体更新一次(cbPerObject)
		mFunction_RenderMeshInList_UpdatePerObject(pMesh);

		//更新完cb就准备开始draw了
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Default);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &pMesh->m_pVB_Gpu, &g_cVBstride_Default, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(pMesh->m_pIB_Gpu, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(pMesh->GetFillMode(), pMesh->GetCullMode());

		//设置blend state
		mFunction_SetBlendState(pMesh->GetBlendMode());

		//设置samplerState
		m_pRefShaderVarMgr->SetSampler(m_pRefShaderVarMgr->NOISE_SHADER_VAR_SAMPLER::DEFAULT, 0, m_pSamplerState_FilterLinear);

		//设置depth/Stencil State
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);


		//for every subset
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
			UINT passID = mFunction_RenderMeshInList_UpdatePerSubset(pMesh,j);

			m_pFX_Tech_Default->GetPassByIndex(passID)->Apply(0, g_pImmediateContext);
			g_pImmediateContext->DrawIndexed(currSubsetIndicesCount, currSubsetStartIndex, 0);
			
		}
	}
#pragma endregion Render Mesh

}


/***********************************************************************
									P R I V A T E
************************************************************************/


void		IRenderer::mFunction_RenderMeshInList_UpdateRarely()
{
	//————更新Static Light——————
	ILightManager* tmpLightMgr = GetScene()->GetLightMgr();

	if (tmpLightMgr->mCanUpdateStaticLights)
	{
		UINT tmpLight_Dir_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_STATIC_DIR);
		UINT tmpLight_Point_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_STATIC_POINT);
		UINT tmpLight_Spot_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_STATIC_SPOT);

		m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::STATIC_LIGHT_ENABLED, tmpLightMgr->IsStaticLightingEnabled());
		m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::STATIC_DIRLIGHT_COUNT, tmpLight_Dir_Count);
		m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::STATIC_POINTLIGHT_COUNT, tmpLight_Point_Count);
		m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::STATIC_SPOTLIGHT_COUNT, tmpLight_Spot_Count);

		for (UINT i = 0; i<(tmpLight_Dir_Count); i++)
		{
			m_pRefShaderVarMgr->SetStaticDirLight(i, tmpLightMgr->GetDirLightS(i)->GetDesc());
		}

		for (UINT i = 0; i<(tmpLight_Point_Count); i++)
		{
			m_pRefShaderVarMgr->SetStaticPointLight(i, tmpLightMgr->GetPointLightS(i)->GetDesc());
		}

		for (UINT i = 0; i<(tmpLight_Spot_Count); i++)
		{
			m_pRefShaderVarMgr->SetStaticSpotLight(i, tmpLightMgr->GetSpotLightS(i)->GetDesc());
		}

		//static light only need to update once for INITIALIZATION / ELIMATION
		tmpLightMgr->mCanUpdateStaticLights = false;
	}
};

void		IRenderer::mFunction_RenderMeshInList_UpdatePerFrame(ICamera*const pCamera)
{
	//————Update Dynamic Light————
	ILightManager* tmpLightMgr = GetScene()->GetLightMgr();

	UINT tmpLight_Dir_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_DIR);
	UINT tmpLight_Point_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_POINT);
	UINT tmpLight_Spot_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_SPOT);

	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_LIGHT_ENABLED, tmpLightMgr->IsDynamicLightingEnabled());
	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_DIRLIGHT_COUNT, tmpLight_Dir_Count);
	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_POINTLIGHT_COUNT, tmpLight_Point_Count);
	m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::DYNAMIC_SPOTLIGHT_COUNT, tmpLight_Spot_Count);

	for (UINT i = 0; i<(tmpLight_Dir_Count); i++)
	{
		m_pRefShaderVarMgr->SetDynamicDirLight(i, tmpLightMgr->GetDirLightD(i)->GetDesc());
	}

	for (UINT i = 0; i<(tmpLight_Point_Count); i++)
	{
		m_pRefShaderVarMgr->SetDynamicPointLight(i, tmpLightMgr->GetPointLightD(i)->GetDesc());
	}

	for (UINT i = 0; i<(tmpLight_Spot_Count); i++)
	{
		m_pRefShaderVarMgr->SetDynamicSpotLight(i, tmpLightMgr->GetSpotLightD(i)->GetDesc());
	}
};

UINT		IRenderer::mFunction_RenderMeshInList_UpdatePerSubset(IMesh* const pMesh,UINT subsetID)
{
	//we dont accept invalid material ,but accept invalid texture
	ITextureManager*		pTexMgr = GetScene()->GetTextureMgr();
	IMaterialManager*		pMatMgr = GetScene()->GetMaterialMgr();

	//Get Material ID by unique name
	N_UID	 currSubsetMatName = pMesh->mSubsetInfoList.at(subsetID).matName;
	bool  IsMatNameValid = pMatMgr->FindUid(currSubsetMatName);

	//if material ID == INVALID_MAT_ID , then we should use default mat defined in mat mgr
	//then we should check if its child textureS are valid too 
	N_MaterialDesc tmpMat;
	if (IsMatNameValid== false)
	{
		WARNING_MSG("IRenderer : material UID not valid when rendering mesh.");
		pMatMgr->GetDefaultMaterial()->GetDesc(tmpMat);
	}
	else
	{
		pMatMgr->GetMaterial(currSubsetMatName)->GetDesc(tmpMat);
	}


	//Validate Indices of MATERIALS/TEXTURES
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
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::DIFFUSE_MAP, pDiffMap->m_pSRV);
	}

	//if tetxure is  valid ,then set normal map
	if (isNormalMapValid)
	{
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::NORMAL_MAP, pNormalMap->m_pSRV);
	}

	//if tetxure is  valid ,then set specular map
	if (isSpecularMapValid)
	{
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::SPECULAR_MAP, pSpecMap->m_pSRV);
	}

	//if tetxure is  valid ,then set environment map (cube map)
	if (isEnvMapValid)
	{
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::CUBE_MAP, pEnvMap->m_pSRV);
	}


	//return pass ID to choose appropriate shader
	//each bit for each switch, then bitwise-AND all the switches
	constexpr int perPixelRenderSwitchCount = 4;

	//NOTE that: special reneder technique like NORMAL MAPPING can only be
	//implemented in TANGENT SPACE
	bool isPerVertexLighting = (pMesh->GetShadeMode() == NOISE_SHADEMODE_GOURAUD);
	if (isPerVertexLighting)return pow(2, perPixelRenderSwitchCount);

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
	return passID;
};

void		IRenderer::mFunction_RenderMeshInList_UpdatePerObject(IMesh* const pMesh)
{
	//update world/worldInv matrix
	NMATRIX worldMat,worldInvTransposeMat;
	pMesh->GetWorldMatrix(worldMat, worldInvTransposeMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD, worldMat);
	m_pRefShaderVarMgr->SetMatrix(IShaderVariableManager::NOISE_SHADER_VAR_MATRIX::WORLD_INV_TRANSPOSE, worldInvTransposeMat);
};

