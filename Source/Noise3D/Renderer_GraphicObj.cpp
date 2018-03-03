
/***********************************************************************

							Desc: Graphic Object Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IRenderModuleForGraphicObject::IRenderModuleForGraphicObject()
{
}

IRenderModuleForGraphicObject::~IRenderModuleForGraphicObject()
{
	ReleaseCOM(m_pFX_Tech_Solid2D);
	ReleaseCOM(m_pFX_Tech_Solid3D);
	ReleaseCOM(m_pFX_Tech_Textured2D);
}

void IRenderModuleForGraphicObject::RenderGraphicObjects()
{
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();

	ICamera* const pCamera = GetScene()->GetCamera();

	//set samplerState
	m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, NOISE_SAMPLERMODE::LINEAR);
	m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DRAW_2D_SAMPLER, NOISE_SAMPLERMODE::LINEAR);
	m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);

	//render various kinds of primitives
	m_pRefRI->SetDepthStencilState(true);
	if (pCamera != nullptr)mFunction_GraphicObj_RenderLine3DInList(pCamera, &mRenderList_GO);
	if (pCamera != nullptr)mFunction_GraphicObj_RenderPoint3DInList(pCamera, &mRenderList_GO);
	m_pRefRI->SetDepthStencilState(false);
	mFunction_GraphicObj_RenderLine2DInList(&mRenderList_GO);
	mFunction_GraphicObj_RenderPoint2DInList(&mRenderList_GO);
	mFunction_GraphicObj_RenderTriangle2DInList(&mRenderList_GO);
}

void IRenderModuleForGraphicObject::AddToRenderQueue(IGraphicObject * pObj)
{
	if (pObj != nullptr)mFunction_AddToRenderList_GraphicObj(pObj,&mRenderList_GO);
}

/***********************************************************************
										PROTECTED
************************************************************************/
void IRenderModuleForGraphicObject::ClearRenderList()
{
	mRenderList_GO.clear();
}

bool IRenderModuleForGraphicObject::Initialize(IRenderInfrastructure * pRI, IShaderVariableManager * pShaderVarMgr)
{
	m_pRefRI = pRI;
	m_pRefShaderVarMgr = pShaderVarMgr;
	m_pFX_Tech_Solid3D = g_pFX->GetTechniqueByName("DrawSolid3D");
	m_pFX_Tech_Solid2D = g_pFX->GetTechniqueByName("DrawSolid2D");
	m_pFX_Tech_Textured2D = g_pFX->GetTechniqueByName("DrawTextured2D");
	return true;
}

/***********************************************************
									P R I V A T E
************************************************************/

void	IRenderModuleForGraphicObject::mFunction_AddToRenderList_GraphicObj(IGraphicObject* pGraphicObj, std::vector<IGraphicObject*>* pList)
{
	pList->push_back(pGraphicObj);

	//Update Data to GPU if data is not up to date , 5 object types for now
	for (UINT i = 0; i < NOISE_GRAPHIC_OBJECT_BUFFER_COUNT; i++)
	{
		if (pGraphicObj->mCanUpdateToGpu[i] == true)
		{
			pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			pGraphicObj->mCanUpdateToGpu[i] = false;
			// rectangles can have textures, thus a subset list should be generated
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderLine3DInList(ICamera*const pCamera,std::vector<IGraphicObject*>* pList)
{
	//update camera related matrix
	m_pRefRI->UpdateCameraMatrix(pCamera);

	for (UINT i = 0;i < pList->size();i++)
	{
		IGraphicObject* pGObj = pList->at(i);
		UINT vCount = pGObj->GetLine3DCount() * 2;
		if (vCount == 0)continue;

		//IA settings
		ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D];
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		m_pRefRI->SetBlendState(pGObj->GetBlendMode());
		m_pRefRI->SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

		//draw
		m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}
}

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderPoint3DInList(ICamera*const pCamera,std::vector<IGraphicObject*>* pList)
{
	//update camera related matrix
	m_pRefRI->UpdateCameraMatrix(pCamera);

	for (UINT i = 0;i < pList->size();i++)
	{
		IGraphicObject* pGObj = pList->at(i);
		UINT vCount = pGObj->GetPoint3DCount();
		if (vCount == 0)continue;

		//IA settings
		ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D];
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_pRefRI->SetBlendState(pGObj->GetBlendMode());
		m_pRefRI->SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//draw
		m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

}

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderLine2DInList(std::vector<IGraphicObject*>* pList)
{
	for (UINT i = 0;i < pList->size();i++)
	{
		IGraphicObject* pGObj = pList->at(i);
		UINT vCount = pGObj->GetLine2DCount() * 2;
		if (vCount == 0)continue;

		//IA settings
		ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D];
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		m_pRefRI->SetBlendState(pGObj->GetBlendMode());
		m_pRefRI->SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

		//draw
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}
};

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderPoint2DInList(std::vector<IGraphicObject*>* pList)
{
	for (UINT i = 0;i < pList->size();i++)
	{
		IGraphicObject* pGObj = pList->at(i);
		UINT vCount = pGObj->GetPoint2DCount() * 2;
		if (vCount == 0)continue;

		//IA settings
		ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D];
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		m_pRefRI->SetBlendState(pGObj->GetBlendMode());
		m_pRefRI->SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//draw
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

};

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderTriangle2DInList(std::vector<IGraphicObject*>* pList)
{
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();

	for (UINT i = 0;i < pList->size();i++)
	{
		IGraphicObject* pGObj = pList->at(i);
		m_pRefRI->SetBlendState(pGObj->GetBlendMode());
		m_pRefRI->SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

		//----------------------1,draw common triangle----------------------
		ID3D11Buffer* tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D];
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//traverse all region list , to decide use which tech to draw (textured or not)
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		UINT vCount = pGObj->GetTriangle2DCount() * 3;
		if (vCount>0)g_pImmediateContext->Draw(pGObj->GetTriangle2DCount() * 3, 0);


		//---------------------2,draw (solid/textured) rectangles---------------------
		tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto tmpRegion : *(pGObj->m_pRectSubsetInfoList))
		{
			//if current Rectangle disable Texture ,then draw in a solid way
			//thus validate the UID first
			if (pTexMgr->ValidateUID(tmpRegion.texName)== false)
			{
				//draw with solid texture
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//current subset have available texture, update SRV
				mFunction_GraphicObj_Update_RenderTextured2D(tmpRegion.texName);
				m_pFX_Tech_Textured2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}

			//draw 
			vCount = tmpRegion.vertexCount;
			if (vCount>0)g_pImmediateContext->Draw(tmpRegion.vertexCount, tmpRegion.startID);
		}

	}
}

//invoked by RenderTriangle(when needed)
void	IRenderModuleForGraphicObject::mFunction_GraphicObj_Update_RenderTextured2D(N_UID texName)
{
	//validate texture type
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();
	bool IsUidValid = pTexMgr->ValidateUID(texName, NOISE_TEXTURE_TYPE_COMMON);

	if (IsUidValid)
	{
		auto*  tmp_pSRV=m_pRefRI->GetTextureSRV(pTexMgr,texName);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::COLOR_MAP_2D, tmp_pSRV);
	}
}
