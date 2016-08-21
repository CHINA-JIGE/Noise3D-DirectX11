
/***********************************************************************

							Desc:  Noise Mesh Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void	IRenderer::RenderMeshes()
{

	ICamera* const tmp_pCamera = GetScene()->GetCamera();

	//更新ConstantBuffer:修改过就更新(cbRarely)
	mFunction_RenderMeshInList_UpdateCbRarely();


	//Update ConstantBuffer: Once Per Frame (cbPerFrame)
	mFunction_RenderMeshInList_UpdateCbPerFrame(tmp_pCamera);

	//Update ConstantBuffer : Proj / View Matrix (this function could be used elsewhere)
	mFunction_CameraMatrix_Update(tmp_pCamera);


#pragma region Render Mesh
	//for every mesh
	for (UINT i = 0; i<(m_pRenderList_Mesh->size()); i++)
	{
		//取出渲染列表中的mesh指针
		IMesh* const tmp_pMesh = m_pRenderList_Mesh->at(i);

		//更新ConstantBuffer:每物体更新一次(cbPerObject)
		mFunction_RenderMeshInList_UpdateCbPerObject(tmp_pMesh);

		//更新完cb就准备开始draw了
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Default);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pMesh->m_pVB_Gpu, &c_VBstride_Default, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(tmp_pMesh->m_pIB_Gpu, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(m_FillMode, m_CullMode);

		//设置blend state
		mFunction_SetBlendState(m_BlendMode);

		//设置samplerState
		m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);

		//设置depth/Stencil State
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);


		//for every subset
		UINT meshSubsetCount = tmp_pMesh->m_pSubsetInfoList->size();
		for (UINT j = 0;j < meshSubsetCount;j++)
		{
			//subset info
			UINT currSubsetIndicesCount = tmp_pMesh->m_pSubsetInfoList->at(j).primitiveCount * 3;
			UINT currSubsetStartIndex = tmp_pMesh->m_pSubsetInfoList->at(j).startPrimitiveID * 3;

			//更新ConstantBuffer:每Subset,在一个mesh里面有不同Material的都算一个subset
			mFunction_RenderMeshInList_UpdateCbPerSubset(tmp_pMesh,j);

			//遍历所用tech的所有pass ---- index starts from 1
			D3DX11_TECHNIQUE_DESC tmpTechDesc;
			m_pFX_Tech_Default->GetDesc(&tmpTechDesc);
			for (UINT k = 0;k < tmpTechDesc.Passes; k++)
			{
				m_pFX_Tech_Default->GetPassByIndex(k)->Apply(0, g_pImmediateContext);
				g_pImmediateContext->DrawIndexed(currSubsetIndicesCount, currSubsetStartIndex, 0);
			}
		}
	}
#pragma endregion Render Mesh

}


/***********************************************************************
									P R I V A T E
************************************************************************/


void		IRenderer::mFunction_RenderMeshInList_UpdateCbRarely()
{

	BOOL tmpCanUpdateCbRarely = FALSE;

	//————更新Static Light——————
	ILightManager* tmpLightMgr = GetScene()->GetLightMgr();

	if ((tmpLightMgr != NULL) && (tmpLightMgr->mCanUpdateStaticLights))
	{
		UINT tmpLight_Dir_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_STATIC_DIR);
		UINT tmpLight_Point_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_STATIC_POINT);
		UINT tmpLight_Spot_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_STATIC_SPOT);

		m_CbRarely.mIsLightingEnabled_Static = tmpLightMgr->mIsDynamicLightingEnabled;
		m_CbRarely.mDirLightCount_Static = tmpLight_Dir_Count;
		m_CbRarely.mPointLightCount_Static = tmpLight_Point_Count;
		m_CbRarely.mSpotLightCount_Static = tmpLight_Spot_Count;


		for (UINT i = 0; i<(tmpLight_Dir_Count); i++)
		{
			//static directional light description
			m_CbRarely.mDirectionalLight_Static[i] = (tmpLightMgr->GetDirLightS(i)->GetDesc());
		}

		for (UINT i = 0; i<(tmpLight_Point_Count); i++)
		{
			m_CbRarely.mPointLight_Static[i] = (tmpLightMgr->GetPointLightS(i)->GetDesc());
		}

		for (UINT i = 0; i<(tmpLight_Spot_Count); i++)
		{
			m_CbRarely.mSpotLight_Static[i] = (tmpLightMgr->GetSpotLightS(i)->GetDesc());
		}

		//更新 “可更新”状态，保证static light 只进行初始化
		tmpLightMgr->mCanUpdateStaticLights = FALSE;
	}


	//——————更新到GPU——————
	if (tmpCanUpdateCbRarely == TRUE)
	{
		m_pFX_CbRarely->SetRawValue(&m_CbRarely, 0, sizeof(m_CbRarely));
	};
};

void		IRenderer::mFunction_RenderMeshInList_UpdateCbPerFrame(ICamera*const pCamera)
{
	//————更新Dynamic Light————
	ILightManager* tmpLightMgr = GetScene()->GetLightMgr();
	if (tmpLightMgr != NULL)
	{
		UINT tmpLight_Dir_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_DIR);
		UINT tmpLight_Point_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_POINT);
		UINT tmpLight_Spot_Count = tmpLightMgr->GetLightCount(NOISE_LIGHT_TYPE_DYNAMIC_SPOT);

		m_CbPerFrame.mIsLightingEnabled_Dynamic = tmpLightMgr->mIsDynamicLightingEnabled;
		m_CbPerFrame.mDirLightCount_Dynamic = tmpLight_Dir_Count;
		m_CbPerFrame.mPointLightCount_Dynamic = tmpLight_Point_Count;
		m_CbPerFrame.mSpotLightCount_Dynamic = tmpLight_Spot_Count;
		m_CbPerFrame.mCamPos = pCamera->GetPosition();

		for (UINT i = 0; i<(tmpLight_Dir_Count); i++)
		{
			m_CbPerFrame.mDirectionalLight_Dynamic[i] = tmpLightMgr->GetDirLightD(i)->GetDesc();
		}

		for (UINT i = 0; i<(tmpLight_Point_Count); i++)
		{
			m_CbPerFrame.mPointLight_Dynamic[i] = tmpLightMgr->GetPointLightD(i)->GetDesc();
		}

		for (UINT i = 0; i<(tmpLight_Spot_Count); i++)
		{
			m_CbPerFrame.mSpotLight_Dynamic[i] = tmpLightMgr->GetSpotLightD(i)->GetDesc();
		}

	}


	//————更新到GPU——————
	m_pFX_CbPerFrame->SetRawValue(&m_CbPerFrame, 0, sizeof(m_CbPerFrame));
};

void		IRenderer::mFunction_RenderMeshInList_UpdateCbPerSubset(IMesh* const pMesh,UINT subsetID)
{
	//we dont accept invalid material ,but accept invalid texture
	ITextureManager*		pTexMgr = GetScene()->GetTextureMgr();
	IMaterialManager*		pMatMgr = GetScene()->GetMaterialMgr();

	//Get Material ID by unique name
	N_UID	 currSubsetMatName = pMesh->m_pSubsetInfoList->at(subsetID).matName;
	BOOL  IsMatNameValid = pMatMgr->FindUid(currSubsetMatName);

	//if material ID == INVALID_MAT_ID , then we should use default mat defined in mat mgr
	//then we should check if its child textureS are valid too 
	N_MaterialDesc tmpMat;
	if (IsMatNameValid==FALSE)
	{
		pMatMgr->GetDefaultMaterial()->GetDesc(tmpMat);
	}
	else
	{
		pMatMgr->GetMaterial(currSubsetMatName)->GetDesc(tmpMat);
	}


	//Validate Indices of MATERIALS/TEXTURES
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;
	//m_CbPerSubset.basicMaterial = tmpMat.baseMaterial;
	m_CbPerSubset.SetBaseMat(tmpMat);

	UINT diffMapIndex = pTexMgr->GetTextureID(tmpMat.diffuseMapName);
	UINT normalMapIndex = pTexMgr->GetTextureID(tmpMat.normalMapName);
	UINT specularMapIndex = pTexMgr->GetTextureID(tmpMat.specularMapName);
	UINT envMapIndex = pTexMgr->GetTextureID(tmpMat.environmentMapName);

	//first validate if ID is valid (within range / valid ID) valid== return original texID
	m_CbPerSubset.IsDiffuseMapValid = (pTexMgr->ValidateIndex(diffMapIndex, NOISE_TEXTURE_TYPE_COMMON)
		== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
	m_CbPerSubset.IsNormalMapValid = (pTexMgr->ValidateIndex(normalMapIndex, NOISE_TEXTURE_TYPE_COMMON)
		== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
	m_CbPerSubset.IsSpecularMapValid = (pTexMgr->ValidateIndex(specularMapIndex, NOISE_TEXTURE_TYPE_COMMON)
		== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);
	m_CbPerSubset.IsEnvironmentMapValid = (pTexMgr->ValidateIndex(envMapIndex, NOISE_TEXTURE_TYPE_CUBEMAP)
		== NOISE_MACRO_INVALID_TEXTURE_ID ? FALSE : TRUE);


	//update textures, bound corresponding ShaderResourceView to the pipeline
	//if tetxure is  valid ,then set diffuse map
	if (m_CbPerSubset.IsDiffuseMapValid)
	{
		tmp_pSRV = pTexMgr->GetObjectPtr(diffMapIndex)->m_pSRV;
		m_pFX_Texture_Diffuse->SetResource(tmp_pSRV);
	}

	//if tetxure is  valid ,then set normal map
	if (m_CbPerSubset.IsNormalMapValid)
	{
		tmp_pSRV = pTexMgr->GetObjectPtr(normalMapIndex)->m_pSRV;
		m_pFX_Texture_Normal->SetResource(tmp_pSRV);
	}

	//if tetxure is  valid ,then set specular map
	if (m_CbPerSubset.IsSpecularMapValid)
	{
		tmp_pSRV = pTexMgr->GetObjectPtr(specularMapIndex)->m_pSRV;
		m_pFX_Texture_Specular->SetResource(tmp_pSRV);
	}

	//if tetxure is  valid ,then set environment map (cube map)
	if (m_CbPerSubset.IsEnvironmentMapValid)
	{
		tmp_pSRV = pTexMgr->GetObjectPtr(envMapIndex)->m_pSRV;
		m_pFX_Texture_CubeMap->SetResource(tmp_pSRV);//environment map is a cube map
	}

	//transmit all data to gpu
	m_pFX_CbPerSubset->SetRawValue(&m_CbPerSubset, 0, sizeof(m_CbPerSubset));

};

void		IRenderer::mFunction_RenderMeshInList_UpdateCbPerObject(IMesh* const pMesh)
{
	//————更新World Matrix————
	pMesh->GetWorldMatrix(m_CbPerObject.mWorldMatrix, m_CbPerObject.mWorldInvTransposeMatrix);
	
	//——————更新到GPU——————
	m_pFX_CbPerObject->SetRawValue(&m_CbPerObject, 0, sizeof(m_CbPerObject));
};

