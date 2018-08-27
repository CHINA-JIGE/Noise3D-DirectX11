
/***********************************************************************

							Desc:  Render Module for mesh
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

Noise3D::IRenderModuleForSweepingTrailFX::IRenderModuleForSweepingTrailFX()
{
}

Noise3D::IRenderModuleForSweepingTrailFX::~IRenderModuleForSweepingTrailFX()
{
	ReleaseCOM(m_pFX_DrawSweepingTrail);
}

void Noise3D::IRenderModuleForSweepingTrailFX::AddToRenderQueue(ISweepingTrail * pTrail)
{
	mRenderList_SweepingTrails.push_back(pTrail);
}

/***********************************************************
									PROTECTED
************************************************************/

void Noise3D::IRenderModuleForSweepingTrailFX::RenderSweepingTrails()
{
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();
	ICamera* const pCamera = GetScene()->GetCamera();
	//update view/proj matrix
	m_pRefRI->UpdateCameraMatrix(pCamera);

	for (auto pTrail : mRenderList_SweepingTrails)
	{
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, pTrail->m_pVB_Gpu, nullptr, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pRefRI->SetRasterState(pTrail->GetFillMode(), NOISE_CULLMODE_NONE);
		m_pRefRI->SetBlendState(pTrail->GetBlendMode());
		m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, NOISE_SAMPLERMODE::LINEAR_CLAMP);
		m_pRefRI->SetDepthStencilState(true);
		m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);

		//update
		mFunction_SweepingTrail_Update(pTrail);

		//draw
		m_pFX_DrawSweepingTrail->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (pTrail->GetActiveVerticesCount()>0)g_pImmediateContext->Draw(pTrail->GetActiveVerticesCount(), 0);

	}
}

void Noise3D::IRenderModuleForSweepingTrailFX::ClearRenderList()
{
	mRenderList_SweepingTrails.clear();
}

bool Noise3D::IRenderModuleForSweepingTrailFX::Initialize(IRenderInfrastructure * pRI, IShaderVariableManager * pShaderVarMgr)
{
	m_pRefRI = pRI;
	m_pRefShaderVarMgr = pShaderVarMgr;
	m_pFX_DrawSweepingTrail = g_pFX->GetTechniqueByName("DrawSweepingTrail");
	return true;
}

/***********************************************************
										PRIVATE
***********************************************************/

void Noise3D::IRenderModuleForSweepingTrailFX::mFunction_SweepingTrail_Update(ISweepingTrail * const pTrail)
{
	//update texture
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();
	ITexture* pTex = pTexMgr->GetTexture(pTrail->GetTextureName());
	if (pTex && pTex->IsTextureType(NOISE_TEXTURE_TYPE_COMMON))
	{
		auto pSRV = m_pRefRI->GetTextureSRV(pTex);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::COLOR_MAP_2D, pSRV);
	}

}
