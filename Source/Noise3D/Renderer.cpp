
/***********************************************************************

										IRenderer
			Composition of infrastructure and render modules

************************************************************************/

#include "Noise3D.h"

Noise3D::IRenderer::IRenderer():
	IFactory<IRenderInfrastructure>(1)
{
}

Noise3D::IRenderer::~IRenderer()
{
	IFactory<IRenderInfrastructure>::DestroyAllObject();
};

void Noise3D::IRenderer::AddToRenderQueue(IMesh * obj)
{
	IRenderModuleForMesh::AddToRenderQueue(obj);
}

void Noise3D::IRenderer::AddToRenderQueue(IGraphicObject * obj)
{
	IRenderModuleForGraphicObject::AddToRenderQueue(obj);
}

void Noise3D::IRenderer::AddToRenderQueue(IDynamicText * obj)
{
	IRenderModuleForText::AddToRenderQueue(obj);
}

void Noise3D::IRenderer::AddToRenderQueue(IStaticText * obj)
{
	IRenderModuleForText::AddToRenderQueue(obj);
}

void Noise3D::IRenderer::SetActiveAtmosphere(IAtmosphere * obj)
{
	IRenderModuleForAtmosphere::SetActiveAtmosphere(obj);
}

void Noise3D::IRenderer::Render()
{
	//this affects the decision of Render Targets
	m_pRenderInfrastructure->SetPostProcessRemainingPassCount(IRenderModuleForPostProcessing::GetPostProcessPassCount());

	IRenderModuleForMesh::RenderMeshes();
	IRenderModuleForAtmosphere::RenderAtmosphere();
	IRenderModuleForGraphicObject::RenderGraphicObjects();
	IRenderModuleForText::RenderTexts();

	IRenderModuleForPostProcessing::PostProcess();
	
}

void	Noise3D::IRenderer::ClearBackground(const NVECTOR4& color)
{
	m_pRenderInfrastructure->ClearRtvAndDsv(color);
};

void	Noise3D::IRenderer::PresentToScreen()
{
	m_pRenderInfrastructure->SwapChainPresent();

	//pop out rendered objects
	IRenderModuleForAtmosphere::ClearRenderList();
	IRenderModuleForGraphicObject::ClearRenderList();
	IRenderModuleForMesh::ClearRenderList();
	IRenderModuleForText::ClearRenderList();
	IRenderModuleForPostProcessing::ClearRenderList();//no use
};


UINT Noise3D::IRenderer::GetBackBufferWidth()
{
	return m_pRenderInfrastructure->GetBackBufferWidth();
};

UINT Noise3D::IRenderer::GetBackBufferHeight()
{
	return m_pRenderInfrastructure->GetBackBufferHeight();
}

HWND Noise3D::IRenderer::GetRenderWindowHWND()
{
	return m_pRenderInfrastructure->GetRenderWindowHWND();
}

uint32_t Noise3D::IRenderer::GetRenderWindowWidth()
{
	return m_pRenderInfrastructure->GetRenderWindowWidth();
}

uint32_t Noise3D::IRenderer::GetRenderWindowHeight()
{
	return m_pRenderInfrastructure->GetRenderWindowHeight();
}

void Noise3D::IRenderer::SwitchToFullScreenMode()
{
	m_pRenderInfrastructure->SwitchToFullScreenMode();
}

void Noise3D::IRenderer::SwitchToWindowedMode()
{
	m_pRenderInfrastructure->SwitchToWindowedMode();
}

/************************************************************************
                                            PRIVATE                        
************************************************************************/
bool	Noise3D::IRenderer::mFunction_Init(UINT BufferWidth, UINT BufferHeight, HWND renderWindowHandle)
{
	//1. try to init a unique render infrastructure 
	//(failure could affect the creation of IRenderer)
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
	IRenderModuleForPostProcessing::Initialize(m_pRenderInfrastructure, pSVM);

	return true;
}


