
/***********************************************************************

										IRenderer
			Composition of infrastructure and render modules

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IRenderer::IRenderer():
	IFactory<IRenderInfrastructure>(1)
{
}

IRenderer::~IRenderer()
{
	IFactory<IRenderInfrastructure>::DestroyAllObject();
};

void IRenderer::AddToRenderQueue(IMesh * obj)
{
	IRenderModuleForMesh::AddToRenderQueue(obj);
}

void IRenderer::AddToRenderQueue(IGraphicObject * obj)
{
	IRenderModuleForGraphicObject::AddToRenderQueue(obj);
}

void IRenderer::AddToRenderQueue(IDynamicText * obj)
{
	IRenderModuleForText::AddToRenderQueue(obj);
}

void IRenderer::AddToRenderQueue(IStaticText * obj)
{
	IRenderModuleForText::AddToRenderQueue(obj);
}

void IRenderer::SetActiveAtmosphere(IAtmosphere * obj)
{
	IRenderModuleForAtmosphere::SetActiveAtmosphere(obj);
}

void IRenderer::Render()
{
	IRenderModuleForMesh::RenderMeshes();
	IRenderModuleForAtmosphere::RenderAtmosphere();
	IRenderModuleForGraphicObject::RenderGraphicObjects();
	IRenderModuleForText::RenderTexts();
}

void	IRenderer::ClearBackground(const NVECTOR4& color)
{
	m_pRenderInfrastructure->ClearRtvAndDsv(color);
};

void	IRenderer::PresentToScreen()
{
	m_pRenderInfrastructure->SwapChainPresent();

	//pop out rendered objects
	IRenderModuleForAtmosphere::ClearRenderList();
	IRenderModuleForGraphicObject::ClearRenderList();
	IRenderModuleForMesh::ClearRenderList();
	IRenderModuleForText::ClearRenderList();
};


UINT IRenderer::GetBackBufferWidth()
{
	return m_pRenderInfrastructure->mBackBufferWidth;
};

UINT IRenderer::GetBackBufferHeight()
{
	return m_pRenderInfrastructure->mBackBufferHeight;
}

void IRenderer::SetPostProcessingEnabled(bool isEnabled)
{
	m_pRenderInfrastructure->mEnablePostProcessing = isEnabled;
};



/************************************************************************
                                            PRIVATE                        
************************************************************************/
bool	IRenderer::mFunction_Init(UINT BufferWidth, UINT BufferHeight, bool IsWindowed)
{
	//1. try to init a unique render infrastructure 
	//(failure could affect the creation of IRenderer)
	static const N_UID uid = "RI";
	if (IFactory<IRenderInfrastructure>::GetObjectCount() == 0)
	{
		m_pRenderInfrastructure = IFactory<IRenderInfrastructure>::CreateObject(uid);
		if (!m_pRenderInfrastructure->Init(BufferWidth, BufferHeight, IsWindowed))
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

	return true;
}


