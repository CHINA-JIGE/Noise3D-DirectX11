
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
	ReleaseCOM(m_pFX_DrawSolid3D);
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
		m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, NOISE_SAMPLERMODE::LINEAR);
		m_pRefRI->SetDepthStencilState(true);
		m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);

		//update
		mFunction_SweepingTrail_Update(pTrail);

		//draw
		m_pFX_DrawSolid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (pTrail->IsRenderable())g_pImmediateContext->Draw(pTrail->GetLastDrawnVerticesCount(), 0);
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
	m_pFX_DrawSolid3D = g_pFX->GetTechniqueByName("DrawSolid3D");
	return true;
}

/***********************************************************
										PRIVATE
***********************************************************/

void Noise3D::IRenderModuleForSweepingTrailFX::mFunction_SweepingTrail_Update(ISweepingTrail * const pTrail)
{
	//update texture
}
