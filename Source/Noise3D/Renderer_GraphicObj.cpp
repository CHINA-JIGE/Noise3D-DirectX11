
/***********************************************************************

							Desc: Graphic Object Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;


IRenderModuleForGraphicObject::IRenderModuleForGraphicObject()
{
}

IRenderModuleForGraphicObject::~IRenderModuleForGraphicObject()
{
	ReleaseCOM(m_pFX_Tech_Solid2D);
	ReleaseCOM(m_pFX_Tech_Solid3D);
	ReleaseCOM(m_pFX_Tech_Textured2D);
}

void IRenderModuleForGraphicObject::AddToRenderQueue(GraphicObject * pObj)
{
	if (pObj != nullptr)mFunction_AddToRenderList_GraphicObj(pObj,&mRenderList_GO);
}

/***********************************************************************
										PROTECTED
************************************************************************/
void IRenderModuleForGraphicObject::RenderGraphicObjects()
{
	TextureManager* pTexMgr = GetScene()->GetTextureMgr();

	Camera* const pCamera = GetScene()->GetCamera();

	//set samplerState
	m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, NOISE_SAMPLERMODE::LINEAR_WRAP);
	m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DRAW_2D_SAMPLER, NOISE_SAMPLERMODE::LINEAR_WRAP);
	m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);

	//render various kinds of primitives
	for (auto pObj : mRenderList_GO)
	{
		m_pRefRI->SetDepthStencilState(true);
		if (pCamera != nullptr)mFunction_GraphicObj_RenderLine3DInList(pCamera, pObj);
		if (pCamera != nullptr)mFunction_GraphicObj_RenderPoint3DInList(pCamera, pObj);
		m_pRefRI->SetDepthStencilState(false);
		mFunction_GraphicObj_RenderLine2D(pObj);
		mFunction_GraphicObj_RenderPoint2D(pObj);
		mFunction_GraphicObj_RenderTriangle2D(pObj);
	}
}

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

void	IRenderModuleForGraphicObject::mFunction_AddToRenderList_GraphicObj(GraphicObject* pGraphicObj, std::vector<GraphicObject*>* pList)
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

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderLine3DInList(Camera*const pCamera, GraphicObject* pGObj)
{
	//update camera related matrix
	m_pRefRI->UpdateCameraMatrix(pCamera);

	UINT vCount = pGObj->GetLine3DCount() * 2;
	if (vCount == 0)return;

	//IA settings
	ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D];
	m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_pRefRI->SetBlendState(pGObj->GetBlendMode());
	m_pRefRI->SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

	//draw
	m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
	if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
}

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderPoint3DInList(Camera*const pCamera, GraphicObject* pGObj)
{
	//update camera related matrix
	m_pRefRI->UpdateCameraMatrix(pCamera);

	UINT vCount = pGObj->GetPoint3DCount();
	if (vCount == 0)return;

	//IA settings
	ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D];
	m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pRefRI->SetBlendState(pGObj->GetBlendMode());
	m_pRefRI->SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

	//draw
	m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
	if (vCount>0)g_pImmediateContext->Draw(vCount, 0);


}

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderLine2D(GraphicObject* pGObj)
{
	UINT vCount = pGObj->GetLine2DCount() * 2;
	if (vCount == 0)return;

	//IA settings
	ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D];
	m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_pRefRI->SetBlendState(pGObj->GetBlendMode());
	m_pRefRI->SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

	//draw
	m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
	if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
};

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderPoint2D(GraphicObject* pGObj)
{
	UINT vCount = pGObj->GetPoint2DCount() * 2;
	if (vCount == 0)return;

	//IA settings
	ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D];
	m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pRefRI->SetBlendState(pGObj->GetBlendMode());
	m_pRefRI->SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

	//draw
	m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
	if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
};

void	IRenderModuleForGraphicObject::mFunction_GraphicObj_RenderTriangle2D(GraphicObject* pGObj)
{
	TextureManager* pTexMgr = GetScene()->GetTextureMgr();

	m_pRefRI->SetBlendState(pGObj->GetBlendMode());
	m_pRefRI->SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

	//----------------------1,draw common triangle----------------------
	ID3D11Buffer* tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D];
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
		if (pTexMgr->ValidateTex2D(tmpRegion.texName) == false)
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

//invoked by RenderTriangle(when needed)
void	IRenderModuleForGraphicObject::mFunction_GraphicObj_Update_RenderTextured2D(N_UID texName)
{
	//validate texture type
	TextureManager* pTexMgr = GetScene()->GetTextureMgr();
	bool IsUidValid = pTexMgr->ValidateTex2D(texName);

	if (IsUidValid)
	{
		ID3D11ShaderResourceView*  tmp_pSRV=m_pRefRI->GetTextureSRV(pTexMgr,texName,IRenderInfrastructure::NOISE_TEXTURE_TYPE::COMMON2D);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::COLOR_MAP_2D, tmp_pSRV);
	}
}
