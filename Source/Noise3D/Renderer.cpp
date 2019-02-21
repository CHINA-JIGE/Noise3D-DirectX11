
/***********************************************************************

										Renderer
			Composition of infrastructure and render modules

************************************************************************/

#include "Noise3D.h"

Noise3D::Renderer::Renderer():
	IFactory<IRenderInfrastructure>(1)
{
}

Noise3D::Renderer::~Renderer()
{
	IFactory<IRenderInfrastructure>::DestroyAllObject();
};

void Noise3D::Renderer::AddToRenderQueue(Mesh * obj)
{
	IRenderModuleForMesh::AddToRenderQueue(obj);
}

void Noise3D::Renderer::AddToRenderQueue(GraphicObject * obj)
{
	IRenderModuleForGraphicObject::AddToRenderQueue(obj);
}

void Noise3D::Renderer::AddToRenderQueue(DynamicText * obj)
{
	IRenderModuleForText::AddToRenderQueue(obj);
}

void Noise3D::Renderer::AddToRenderQueue(StaticText * obj)
{
	IRenderModuleForText::AddToRenderQueue(obj);
}

void Noise3D::Renderer::AddToRenderQueue(SweepingTrail * obj)
{
	IRenderModuleForSweepingTrailFX::AddToRenderQueue(obj);
}

void Noise3D::Renderer::SetActiveAtmosphere(Atmosphere * obj)
{
	IRenderModuleForAtmosphere::SetActiveAtmosphere(obj);
}

void Noise3D::Renderer::Render()
{
	//this affects the decision of Render Targets
	m_pRenderInfrastructure->SetPostProcessRemainingPassCount(IRenderModuleForPostProcessing::GetPostProcessPassCount());

	//render shadow maps for solid objects
	//(the Shadow map RT belongs to each light, so lights should be the dominant factor of SM rendering
	LightManager* pLM = GetScene()->GetLightMgr();
	pLM->mFunction_GetShadowMapRenderTaskList();

	IRenderModuleForMesh::RenderMeshes();
	IRenderModuleForAtmosphere::RenderAtmosphere();
	IRenderModuleForSweepingTrailFX::RenderSweepingTrails();
	IRenderModuleForGraphicObject::RenderGraphicObjects();
	IRenderModuleForText::RenderTexts();

	IRenderModuleForPostProcessing::PostProcess();
	
}

void	Noise3D::Renderer::ClearBackground(const NVECTOR4& color)
{
	m_pRenderInfrastructure->ClearRtvAndDsv(color);
};

void	Noise3D::Renderer::PresentToScreen()
{
	m_pRenderInfrastructure->SwapChainPresent();

	//pop out rendered objects
	IRenderModuleForAtmosphere::ClearRenderList();
	IRenderModuleForGraphicObject::ClearRenderList();
	IRenderModuleForMesh::ClearRenderList();
	IRenderModuleForText::ClearRenderList();
	IRenderModuleForSweepingTrailFX::ClearRenderList();
	IRenderModuleForPostProcessing::ClearRenderList();//no use
};


UINT Noise3D::Renderer::GetBackBufferWidth()
{
	return m_pRenderInfrastructure->GetBackBufferWidth();
};

UINT Noise3D::Renderer::GetBackBufferHeight()
{
	return m_pRenderInfrastructure->GetBackBufferHeight();
}

HWND Noise3D::Renderer::GetRenderWindowHWND()
{
	return m_pRenderInfrastructure->GetRenderWindowHWND();
}

uint32_t Noise3D::Renderer::GetRenderWindowWidth()
{
	return m_pRenderInfrastructure->GetRenderWindowWidth();
}

uint32_t Noise3D::Renderer::GetRenderWindowHeight()
{
	return m_pRenderInfrastructure->GetRenderWindowHeight();
}

void Noise3D::Renderer::SwitchToFullScreenMode()
{
	m_pRenderInfrastructure->SwitchToFullScreenMode();
}

void Noise3D::Renderer::SwitchToWindowedMode()
{
	m_pRenderInfrastructure->SwitchToWindowedMode();
}

/************************************************************************
                                            PRIVATE                        
************************************************************************/
bool	Noise3D::Renderer::mFunction_Init(UINT BufferWidth, UINT BufferHeight, HWND renderWindowHandle)
{
	//1. try to init a unique render infrastructure 
	//(failure could affect the creation of Renderer)
	static const N_UID uid = "RI";
	if (IFactory<IRenderInfrastructure>::GetObjectCount() == 0)
	{
		m_pRenderInfrastructure = IFactory<IRenderInfrastructure>::CreateObject(uid);
		if (!m_pRenderInfrastructure->Init(BufferWidth, BufferHeight,renderWindowHandle))
		{
			IFactory<IRenderInfrastructure>::DestroyObject(uid);
			m_pRenderInfrastructure = nullptr;
			ERROR_MSG("IRenderer: failed to init render infrastructure.");
			return false;
		}
	}

	//2. Init Render Modules
	IShaderVariableManager* pSVM = m_pRenderInfrastructure->GetRefToShaderVarMgr();
	IRenderModuleForAtmosphere::Initialize(m_pRenderInfrastructure, pSVM);
	IRenderModuleForGraphicObject::Initialize(m_pRenderInfrastructure, pSVM);
	IRenderModuleForMesh::Initialize(m_pRenderInfrastructure, pSVM);
	IRenderModuleForText::Initialize(m_pRenderInfrastructure, pSVM);
	IRenderModuleForSweepingTrailFX::Initialize(m_pRenderInfrastructure, pSVM);
	IRenderModuleForPostProcessing::Initialize(m_pRenderInfrastructure, pSVM);

	return true;
}


