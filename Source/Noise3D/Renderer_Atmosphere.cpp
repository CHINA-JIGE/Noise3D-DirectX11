
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

		if (pAtmo == nullptr)continue;

		//enable/disable fog effect 
		mFunction_Atmosphere_UpdateFogParameters(pAtmo);

#pragma region Draw Sky

		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &pAtmo->m_pVB_Gpu, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(pAtmo->m_pIB_Gpu, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//......Set States
		mFunction_SetRasterState(NOISE_FILLMODE_SOLID , NOISE_CULLMODE_BACK );
		mFunction_SetBlendState(NOISE_BLENDMODE_OPAQUE);
		m_pRefShaderVarMgr->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT, 0, m_pSamplerState_FilterLinear);
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);


		//update Vertices or atmo param to GPU
		//shader will be chosen to render skybox OR skydome
		bool enableSkyDome=false, enableSkyBox=false;
		mFunction_Atmosphere_UpdateSkyParameters(pAtmo,enableSkyBox,enableSkyDome);


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

#pragma endregion Draw Sky
	}

}


/***********************************************************************
									P R I V A T E
************************************************************************/


void		IRenderer::mFunction_Atmosphere_UpdateFogParameters(IAtmosphere*const pAtmo)
{
	if (pAtmo->mFogCanUpdateToGpu)
	{
		//udpate to GPU
		m_pRefShaderVarMgr->SetVector3(IShaderVariableManager::NOISE_SHADER_VAR_VECTOR::FOG_COLOR3, pAtmo->mFogColor);
		m_pRefShaderVarMgr->SetFloat(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::FOG_FAR, pAtmo->mFogFar);
		m_pRefShaderVarMgr->SetFloat(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::FOG_NEAR, pAtmo->mFogNear);
		m_pRefShaderVarMgr->SetInt(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::FOG_ENABLED, pAtmo->mFogEnabled);
		pAtmo->mFogCanUpdateToGpu = false;
	}
};

void		IRenderer::mFunction_Atmosphere_UpdateSkyParameters(IAtmosphere*const pAtmo, bool& outEnabledSkybox, bool& outEnabledSkydome)
{
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();
	N_UID skyTexName = pAtmo->GetSkyTextureUID();
	bool enableSkyBox=false, enableSkyDome = false;

	//check skyType and update corresponding shader variables
	if (pAtmo->mSkyType == NOISE_ATMOSPHERE_SKYTYPE_DOME)
	{
		//if texture pass UID validation and match current skytype
		bool isSkyDomeValid = pTexMgr->ValidateUID(skyTexName, NOISE_TEXTURE_TYPE_COMMON);
		enableSkyDome =  isSkyDomeValid;
	}
	else	if (pAtmo->mSkyType == NOISE_ATMOSPHERE_SKYTYPE_BOX)
	{
		//skybox texture must be a cube map
		bool isSkyBoxValid = pTexMgr->ValidateUID(skyTexName, NOISE_TEXTURE_TYPE_CUBEMAP);
		m_pRefShaderVarMgr->SetFloat(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::SKYBOX_WIDTH, pAtmo->GetSkyboxWidth());
		m_pRefShaderVarMgr->SetFloat(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::SKYBOX_HEIGHT, pAtmo->GetSkyboxHeight());
		m_pRefShaderVarMgr->SetFloat(IShaderVariableManager::NOISE_SHADER_VAR_SCALAR::SKYBOX_DEPTH, pAtmo->GetSkyboxDepth());
		enableSkyBox = isSkyBoxValid;
	}

	//UPDATE TEXTURES
	if (enableSkyDome)
	{
		//texName has been validated in UPDATE function
		auto tmp_pSRV = pTexMgr->GetObjectPtr(skyTexName)->m_pSRV;
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::DIFFUSE_MAP, tmp_pSRV);
	}

	//update skybox cube map to gpu
	if (enableSkyBox)
	{
		//pAtmo->mSkyBoxTextureID has been validated  in UPDATE function
		//but how do you validate it's a valid cube map ?????
		auto tmp_pSRV = pTexMgr->GetObjectPtr(skyTexName)->m_pSRV;
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::CUBE_MAP, tmp_pSRV);
	}

	outEnabledSkybox = enableSkyBox;
	outEnabledSkydome = enableSkyDome;
};
