
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
}

/***********************************************************
									PROTECTED
************************************************************/

void Noise3D::IRenderModuleForSweepingTrailFX::RenderSweepingTrails()
{
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
}

/***********************************************************
										PRIVATE
***********************************************************/

void Noise3D::IRenderModuleForSweepingTrailFX::mFunction_SweepingTrail_Update(ISweepingTrail * const pTrail)
{
}
