
/***********************************************************************

							Desc: Graphic Object Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;;

void IRenderer::RenderGUIObjects()
{
	//validation before rendering
	if (m_pFatherScene->m_pChildTextureMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Texture Mgr has not been created");
		return;
	};

	//CLEAR DEPTH!! to implement component overlapping
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);
	mFunction_SetBlendState(m_BlendMode);
	m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);
	g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);

	//render internal graphic objects
	mFunction_GraphicObj_RenderPoint2DInList(m_pRenderList_GUIGraphicObj);
	mFunction_GraphicObj_RenderLine2DInList(m_pRenderList_GUIGraphicObj);
	mFunction_GraphicObj_RenderTriangle2DInList(m_pRenderList_GUIGraphicObj);
	mFunction_TextGraphicObj_Render(m_pRenderList_GUIText);
}

void IRenderer::RenderGraphicObjects()
{
	//validation before rendering
	if (m_pFatherScene->m_pChildTextureMgr == nullptr)
	{
		DEBUG_MSG1("Noise Renderer : Texture Mgr has not been created");
		return;
	};

	ICamera* const pCamera = m_pFatherScene->GetCamera();

	//set fillmode & cullmode
	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

	//set blend state
	mFunction_SetBlendState(m_BlendMode);

	//set samplerState
	m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);

	//set depth/Stencil State
	g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);

	//render various kinds of primitives
	mFunction_GraphicObj_RenderLine2DInList(m_pRenderList_CommonGraphicObj);
	if (pCamera != nullptr)mFunction_GraphicObj_RenderLine3DInList(pCamera,m_pRenderList_CommonGraphicObj);
	if (pCamera != nullptr)mFunction_GraphicObj_RenderPoint3DInList(pCamera,m_pRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderPoint2DInList(m_pRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderTriangle2DInList(m_pRenderList_CommonGraphicObj);
}

/***********************************************************************
									P R I V A T E
************************************************************************/


void		IRenderer::mFunction_GraphicObj_Update_RenderTextured2D(UINT TexID)
{
	//Get Shader Resource View
	ID3D11ShaderResourceView* tmp_pSRV = NULL;

	//......
	TexID = m_pFatherScene->m_pChildTextureMgr->ValidateIndex(TexID, NOISE_TEXTURE_TYPE_COMMON);

	if (TexID != NOISE_MACRO_INVALID_TEXTURE_ID)
	{
		tmp_pSRV = m_pFatherScene->m_pChildTextureMgr->m_pTextureObjectList->at(TexID).m_pSRV;
		m_pFX2D_Texture_Diffuse->SetResource(tmp_pSRV);
	}
}

void		IRenderer::mFunction_GraphicObj_RenderLine3DInList(ICamera*const pCamera,std::vector<IGraphicObject*>* pList)
{
	//更新ConstantBuffer : Proj / View Matrix
	mFunction_CameraMatrix_Update(pCamera);

	//更新完cb就可以开始draw了
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->GetLine3DCount() * 2;
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &c_VBstride_Simple, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}


}

void		IRenderer::mFunction_GraphicObj_RenderPoint3DInList(ICamera*const pCamera,std::vector<IGraphicObject*>* pList)
{

	//update view/proj matrix
	mFunction_CameraMatrix_Update(pCamera);

	//更新完cb就可以开始draw了
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->GetPoint3DCount();
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &c_VBstride_Simple, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid3D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

}

void		IRenderer::mFunction_GraphicObj_RenderLine2DInList(std::vector<IGraphicObject*>* pList)
{

	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->GetLine2DCount() * 2;
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &c_VBstride_Simple, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

};

void		IRenderer::mFunction_GraphicObj_RenderPoint2DInList(std::vector<IGraphicObject*>* pList)
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;
	for (UINT i = 0;i < pList->size();i++)
	{
		UINT vCount = pList->at(i)->GetPoint2DCount() * 2;
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &c_VBstride_Simple, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//draw line 一个pass就够了
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

};

void		IRenderer::mFunction_GraphicObj_RenderTriangle2DInList(std::vector<IGraphicObject*>* pList)
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;

	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);


	for (UINT i = 0;i < pList->size();i++)
	{
		//----------------------1,draw common triangle----------------------
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &c_VBstride_Simple, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT j = 0, vCount = 0;
		//traverse all region list , to decide use which tech to draw (textured or not)

		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		vCount = pList->at(i)->GetTriangle2DCount() * 3;
		if (vCount>0)g_pImmediateContext->Draw(pList->at(i)->GetTriangle2DCount() * 3, 0);


		//---------------------2,draw rectangles---------------------
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &c_VBstride_Simple, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto tmpRegion : *(pList->at(i)->m_pRectSubsetInfoList))
		{
			//if current Rectangle disable Texture ,then draw in a solid way
			if (tmpRegion.texID == NOISE_MACRO_INVALID_TEXTURE_ID)
			{
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//------Draw 2D Common Texture-----
				mFunction_GraphicObj_Update_RenderTextured2D(tmpRegion.texID);
				m_pFX_Tech_Textured2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}

			//draw 
			vCount = tmpRegion.vertexCount;
			if (vCount>0)g_pImmediateContext->Draw(tmpRegion.vertexCount, tmpRegion.startID);
		}

	}

	//清空渲染列表
	pList->clear();
}
