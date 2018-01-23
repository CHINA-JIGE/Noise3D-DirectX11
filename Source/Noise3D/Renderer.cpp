
/***********************************************************************

										IRenderer
			Composition of infrastructure and render modules

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IRenderer::IRenderer():
	IFactory<IRenderInfrastructure>(1),
	IRenderModuleForAtmosphere()
{
}

IRenderer::~IRenderer()
{
	IFactory<IRenderInfrastructure>::DestroyAllObject();
};


/*void IRenderer::AddObjectToRenderList(IMesh* obj)
{
	mRenderList_Mesh.push_back(obj);
};

void IRenderer::AddObjectToRenderList(IAtmosphere* obj)
{
	mRenderList_Atmosphere.push_back(obj);
};

void IRenderer::AddObjectToRenderList(IGraphicObject* obj)
{
	mFunction_AddToRenderList_GraphicObj(obj, &mRenderList_CommonGraphicObj);
}

void IRenderer::AddObjectToRenderList(IDynamicText* obj)
{
	mFunction_AddToRenderList_Text(obj, &mRenderList_TextDynamic);
}

void IRenderer::AddObjectToRenderList(IStaticText* obj)
{
	mFunction_AddToRenderList_Text(obj, &mRenderList_TextStatic);
};*/

void	IRenderer::ClearBackground(const NVECTOR4& color)
{
	m_pRenderInfrastructure->ClearRtvAndDsv(color);
};

void	IRenderer::PresentToScreen()
{
	m_pRenderInfrastructure->SwapChainPresent();

	IRenderModuleForAtmosphere::ClearRenderList();
		//clear render list
		mRenderList_CommonGraphicObj.clear();
		mRenderList_Mesh.clear();
		mRenderList_TextDynamic.clear();
		mRenderList_TextStatic.clear();
};


UINT IRenderer::GetBackBufferWidth()
{
	return m_pRenderInfrastructure->mBackBufferWidth;
};

UINT IRenderer::GetBackBufferHeight()
{
	return m_pRenderInfrastructure->mBackBufferHeight;
}

void IRenderer::SetDepthTestEnabled(bool isEnabled)
{
	m_pRenderInfrastructure->mEnableDepthTest = isEnabled;
}
void IRenderer::SetPostProcessingEnabled(bool isEnabled)
{

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



	return true;
}

void		IRenderer::mFunction_AddToRenderList_GraphicObj(IGraphicObject* pGraphicObj, std::vector<IGraphicObject*>* pList)
{
	pList->push_back(pGraphicObj);

	//Update Data to GPU if data is not up to date , 5 object types for now
	for (UINT i = 0;i < NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		if (pGraphicObj->mCanUpdateToGpu[i]==true)
		{
			pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			pGraphicObj->mCanUpdateToGpu[i] = false;
			// rectangles can have textures, thus a subset list should be generated
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

void		IRenderer::mFunction_AddToRenderList_Text(IBasicTextInfo * pText, std::vector<IBasicTextInfo*>* pList)
{
	pText->mFunction_UpdateGraphicObject();//implemented by derived Text Class
	pList->push_back(pText);
	//Update Data to GPU if data is not up to date , 6 object types for now
	for (UINT i = 0;i <NOISE_GRAPHIC_OBJECT_BUFFER_COUNT;i++)
	{
		if (pText->m_pGraphicObj->mCanUpdateToGpu[i])
		{
			pText->m_pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			pText->m_pGraphicObj->mCanUpdateToGpu[i] = false;
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)pText->m_pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

