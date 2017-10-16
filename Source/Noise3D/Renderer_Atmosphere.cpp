
/***********************************************************************

							Desc: Atmosphere Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void IRenderer::RenderAtmosphere()
{
	//...................
	ICamera* const tmp_pCamera = GetScene()->GetCamera();

	//update view/proj matrix
	mFunction_CameraMatrix_Update(tmp_pCamera);

	//actually there is only 1 atmosphere because you dont need more 
	for (UINT i = 0;i < m_pRenderList_Atmosphere->size();i++)
	{
		IAtmosphere* const  pAtmo = m_pRenderList_Atmosphere->at(i);
		//texture manager singleton
		ITextureManager* pTexMgr = GetScene()->GetTextureMgr();

		if (pAtmo == nullptr)continue;

		//enable/disable fog effect 
		mFunction_Atmosphere_Fog_Update(pAtmo,pTexMgr);

#pragma region Draw Sky

		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &pAtmo->m_pVB_Gpu, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(pAtmo->m_pIB_Gpu, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//......Set States
		mFunction_SetRasterState(NOISE_FILLMODE_SOLID , NOISE_CULLMODE_BACK );
		mFunction_SetBlendState(NOISE_BLENDMODE_OPAQUE);
		m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterLinear);
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);


		//update Vertices or atmo param to GPU
		//shader will be chosen to render skybox OR skydome

		N_UID skyDomeTexName = "";
		N_UID skyBoxTexName = "";
		bool enableSkyDome =  mFunction_Atmosphere_SkyDome_Update(pAtmo, pTexMgr,skyDomeTexName);
		bool enableSkyBox = mFunction_Atmosphere_SkyBox_Update(pAtmo, pTexMgr,skyBoxTexName);
		mFunction_Atmosphere_UpdateCbAtmosphere(pAtmo, pTexMgr,enableSkyBox,enableSkyDome, skyDomeTexName, skyBoxTexName);


		//traverse passes in one technique ---- pass index starts from 1
		D3DX11_TECHNIQUE_DESC	tmpTechDesc;
		m_pFX_Tech_DrawSky->GetDesc(&tmpTechDesc);

		if (!enableSkyBox && !enableSkyDome)
		{
			//"EmptySky"
			m_pFX_Tech_DrawSky->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			g_pImmediateContext->DrawIndexed(pAtmo->mIB_Mem.size(), 0, 0);
		}
		if (enableSkyBox)
		{
			//"DrawSkyBox"
			m_pFX_Tech_DrawSky->GetPassByIndex(1)->Apply(0, g_pImmediateContext);
			g_pImmediateContext->DrawIndexed(pAtmo->mIB_Mem.size(), 0, 0);
		}
		if (enableSkyDome)
		{
			//"DrawSkyDome"
			m_pFX_Tech_DrawSky->GetPassByIndex(2)->Apply(0, g_pImmediateContext);
			g_pImmediateContext->DrawIndexed(pAtmo->mIB_Mem.size(), 0, 0);
		}

		//allow atmosphere to "add to render list" again 
		pAtmo->mFogHasBeenAddedToRenderList = false;

#pragma endregion Draw Sky
	}

}


/***********************************************************************
									P R I V A T E
************************************************************************/


void		IRenderer::mFunction_Atmosphere_Fog_Update(IAtmosphere*const pAtmo,ITextureManager* const pTexMgr)
{
	if (pAtmo->mFogCanUpdateToGpu)
	{
		//update fog param
		m_CbAtmosphere.mFogColor = pAtmo->mFogColor;
		m_CbAtmosphere.mFogFar = pAtmo->mFogFar;
		m_CbAtmosphere.mFogNear = pAtmo->mFogNear;
		m_CbAtmosphere.mIsFogEnabled = (bool)(pAtmo->mFogEnabled && pAtmo->mFogHasBeenAddedToRenderList);

		//udpate to GPU
		m_pFX_CbAtmosphere->SetRawValue(&m_CbAtmosphere, 0, sizeof(m_CbAtmosphere));
		pAtmo->mFogCanUpdateToGpu = false;
	}
};

bool		IRenderer::mFunction_Atmosphere_SkyDome_Update(IAtmosphere*const pAtmo, ITextureManager* const pTexMgr, N_UID& outSkyDomeTexName)
{
	//validate texture and update bool value to gpu
	 N_UID skyDomeTexName = pAtmo->mSkyDomeTexName;

	//check skyType
	if (pAtmo->mSkyType == NOISE_ATMOSPHERE_SKYTYPE_DOME)
	{
		bool isTextureUidValid = pTexMgr->FindUid(skyDomeTexName);
		//if texture pass UID validation and match current skytype
		bool isSkyDomeValid = pTexMgr->ValidateUID(skyDomeTexName, NOISE_TEXTURE_TYPE_COMMON);
		outSkyDomeTexName = skyDomeTexName;
		return isSkyDomeValid;
	}
	else
	{
		outSkyDomeTexName = "";
		return false;
	}
};

bool		IRenderer::mFunction_Atmosphere_SkyBox_Update(IAtmosphere*const pAtmo, ITextureManager* const pTexMgr, N_UID& outSkyBoxTexName)
{
	//skybox uses cube map to texture the box
	N_UID skyboxTexName =pAtmo->mSkyBoxCubeTexName;

	//check skyType
	if (pAtmo->mSkyType == NOISE_ATMOSPHERE_SKYTYPE_BOX)
	{
		bool isTextureUidValid = pTexMgr->FindUid(skyboxTexName);
		//skybox texture must be a cube map
		bool isSkyBoxValid = pTexMgr->ValidateUID(skyboxTexName, NOISE_TEXTURE_TYPE_CUBEMAP);
		m_CbAtmosphere.mSkyBoxWidth = pAtmo->mSkyBoxWidth;
		m_CbAtmosphere.mSkyBoxHeight = pAtmo->mSkyBoxHeight;
		m_CbAtmosphere.mSkyBoxDepth = pAtmo->mSkyBoxDepth;
		outSkyBoxTexName = skyboxTexName;
		return isSkyBoxValid;
	}
	else
	{
		outSkyBoxTexName = "";
		return false;
	}

};

void		IRenderer::mFunction_Atmosphere_UpdateCbAtmosphere(IAtmosphere*const pAtmo, ITextureManager* const pTexMgr, bool enableSkyBox, bool enableSkyDome, const N_UID& skyDomeTexName, const N_UID& skyBoxTexName)
{
	//update valid texture to gpu
	if (enableSkyDome)
	{
		//texName has been validated in UPDATE function
		auto tmp_pSRV = pTexMgr->GetObjectPtr(skyDomeTexName)->m_pSRV;
		m_pFX_Texture_Diffuse->SetResource(tmp_pSRV);
	}


	//update skybox cube map to gpu
	if (enableSkyBox)
	{
		//pAtmo->mSkyBoxTextureID has been validated  in UPDATE function
		//but how do you validate it's a valid cube map ?????
		auto tmp_pSRV = pTexMgr->GetObjectPtr(skyBoxTexName)->m_pSRV;
		m_pFX_Texture_CubeMap->SetResource(tmp_pSRV);
	}


	m_pFX_CbAtmosphere->SetRawValue(&m_CbAtmosphere, 0, sizeof(m_CbAtmosphere));
};
