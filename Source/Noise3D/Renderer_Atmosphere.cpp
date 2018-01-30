
/***********************************************************************

							Desc: Atmosphere Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IRenderModuleForAtmosphere::IRenderModuleForAtmosphere()
{
}

IRenderModuleForAtmosphere::~IRenderModuleForAtmosphere()
{
	ReleaseCOM(m_pFX_Tech_DrawSky);
}

void IRenderModuleForAtmosphere::SetActiveAtmosphere(IAtmosphere * pAtmo)
{
	if (pAtmo == nullptr)return;
	mRenderList_Atmosphere.resize(1);
	mRenderList_Atmosphere.at(0) = pAtmo;
}

/***********************************************************************
										PROTECTED
************************************************************************/

void IRenderModuleForAtmosphere::RenderAtmosphere()
{
	//...................
	ICamera* const tmp_pCamera = GetScene()->GetCamera();

	//update view/proj matrix
	m_pRefRI->UpdateCameraMatrix(tmp_pCamera);

	//actually there is only 1 atmosphere because you dont need more 
	for (UINT i = 0; i < mRenderList_Atmosphere.size(); i++)
	{
		IAtmosphere* const  pAtmo = mRenderList_Atmosphere.at(i);

		if (pAtmo == nullptr)continue;

		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, pAtmo->m_pVB_Gpu, pAtmo->m_pIB_Gpu, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pRefRI->SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_BACK);
		m_pRefRI->SetBlendState(NOISE_BLENDMODE_OPAQUE);
		m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, NOISE_SAMPLERMODE::LINEAR);
		m_pRefRI->SetDepthStencilState(true);
		m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);

		//enable/disable fog effect 
		mFunction_Atmosphere_UpdateFogParameters(pAtmo);

		//update Vertices or atmo param to GPU
		//shader will be chosen to render skybox OR skydome
		bool enableSkyDome = false, enableSkyBox = false;
		mFunction_Atmosphere_UpdateSkyParameters(pAtmo, enableSkyBox, enableSkyDome);

		//select pass to apply
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
	}
}

void IRenderModuleForAtmosphere::ClearRenderList()
{
	mRenderList_Atmosphere.clear();
}

bool IRenderModuleForAtmosphere::Initialize(IRenderInfrastructure* pRI, IShaderVariableManager* pShaderVarMgr)
{
	m_pRefRI = pRI;
	m_pRefShaderVarMgr = pShaderVarMgr;
	m_pFX_Tech_DrawSky = g_pFX->GetTechniqueByName("DrawSky");
	return true;
}

/***********************************************************************
									P R I V A T E
************************************************************************/

void	IRenderModuleForAtmosphere::mFunction_Atmosphere_UpdateFogParameters(IAtmosphere*const pAtmo)
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

void	IRenderModuleForAtmosphere::mFunction_Atmosphere_UpdateSkyParameters(IAtmosphere*const pAtmo, bool& outEnabledSkybox, bool& outEnabledSkydome)
{
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();
	N_UID skyTexName = pAtmo->GetSkyTextureUID();
	bool enableSkyBox=false, enableSkyDome = false;

	//check skyType and update corresponding shader variables
	if (pAtmo->GetSkyType() == NOISE_ATMOSPHERE_SKYTYPE_DOME)
	{
		//if texture pass UID validation and match current skytype
		bool isSkyDomeValid = pTexMgr->ValidateUID(skyTexName, NOISE_TEXTURE_TYPE_COMMON);
		enableSkyDome =  isSkyDomeValid;
	}
	else	if (pAtmo->GetSkyType() == NOISE_ATMOSPHERE_SKYTYPE_BOX)
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
		auto tmp_pSRV = m_pRefRI->GetTextureSRV(pTexMgr,skyTexName);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::DIFFUSE_MAP, tmp_pSRV);
	}

	//update skybox cube map to gpu
	if (enableSkyBox)
	{
		//pAtmo->mSkyBoxTextureID has been validated  in UPDATE function
		//but how do you validate it's a valid cube map ?????
		auto tmp_pSRV = m_pRefRI->GetTextureSRV(pTexMgr,skyTexName);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::CUBE_MAP, tmp_pSRV);
	}

	outEnabledSkybox = enableSkyBox;
	outEnabledSkydome = enableSkyDome;
};

