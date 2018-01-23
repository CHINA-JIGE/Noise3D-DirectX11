
/***********************************************************************

							Desc: Graphic Object Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void IRenderer::RenderGraphicObjects()
{
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();

	ICamera* const pCamera = GetScene()->GetCamera();

	//set samplerState
	m_pRefShaderVarMgr->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DEFAULT_SAMPLER, 0, m_pSamplerState_FilterLinear);

	//set depth/Stencil State
	g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);

	//Set Render target
	g_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetViewForDisplay, m_pDepthStencilView);

	//render various kinds of primitives
	mFunction_GraphicObj_RenderLine2DInList(&mRenderList_CommonGraphicObj);
	if (pCamera != nullptr)mFunction_GraphicObj_RenderLine3DInList(pCamera,&mRenderList_CommonGraphicObj);
	if (pCamera != nullptr)mFunction_GraphicObj_RenderPoint3DInList(pCamera,&mRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderPoint2DInList(&mRenderList_CommonGraphicObj);
	mFunction_GraphicObj_RenderTriangle2DInList(&mRenderList_CommonGraphicObj);
}

/***********************************************************************
									P R I V A T E
************************************************************************/

//invoked by RenderTriangle(when needed)
void		IRenderer::mFunction_GraphicObj_Update_RenderTextured2D(N_UID texName)
{
	//validate texture type
	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();
	bool IsUidValid = pTexMgr->ValidateUID(texName, NOISE_TEXTURE_TYPE_COMMON);

	if (IsUidValid)
	{
		ID3D11ShaderResourceView*  tmp_pSRV = pTexMgr->GetObjectPtr(texName)->m_pSRV;
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::COLOR_MAP_2D, tmp_pSRV);
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
		IGraphicObject* pGObj = pList->at(i);

		UINT vCount = pGObj->GetLine3DCount() * 2;
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


		//set blend state
		mFunction_SetBlendState(pGObj->GetBlendMode());

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
		IGraphicObject* pGObj = pList->at(i);

		UINT vCount = pGObj->GetPoint3DCount();
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//set fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//set blend state
		mFunction_SetBlendState(pGObj->GetBlendMode());

		//draw point 一个pass就够了
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
		IGraphicObject* pGObj = pList->at(i);

		UINT vCount = pGObj->GetLine2DCount() * 2;
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		//set fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_WIREFRAME, NOISE_CULLMODE_NONE);

		//set blend state
		mFunction_SetBlendState(pGObj->GetBlendMode());

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
		IGraphicObject* pGObj = pList->at(i);

		UINT vCount = pGObj->GetPoint2DCount() * 2;
		if (vCount == 0)continue;

		//settings
		tmp_pVB = pGObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		//set fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_POINT, NOISE_CULLMODE_NONE);

		//set blend state
		mFunction_SetBlendState(pGObj->GetBlendMode());

		//draw point 一个pass就够了
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		if (vCount>0)g_pImmediateContext->Draw(vCount, 0);
	}

};

void		IRenderer::mFunction_GraphicObj_RenderTriangle2DInList(std::vector<IGraphicObject*>* pList)
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;

	ITextureManager* pTexMgr = GetScene()->GetTextureMgr();

	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

	for (UINT i = 0;i < pList->size();i++)
	{
		IGraphicObject* pGObj = pList->at(i);

		//set blend state
		mFunction_SetBlendState(pGObj->GetBlendMode());

		//----------------------1,draw common triangle----------------------
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT vCount = 0;
		//traverse all region list , to decide use which tech to draw (textured or not)
		m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
		vCount = pList->at(i)->GetTriangle2DCount() * 3;
		if (vCount>0)g_pImmediateContext->Draw(pList->at(i)->GetTriangle2DCount() * 3, 0);


		//---------------------2,draw rectangles---------------------
		tmp_pVB = pList->at(i)->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto tmpRegion : *(pList->at(i)->m_pRectSubsetInfoList))
		{
			//if current Rectangle disable Texture ,then draw in a solid way
			//thus validate the UID first
			if (pTexMgr->FindUid(tmpRegion.texName)== false)
			{
				//draw with solid texture
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//------Draw 2D Common Texture-----
				mFunction_GraphicObj_Update_RenderTextured2D(tmpRegion.texName);
				m_pFX_Tech_Textured2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}

			//draw 
			vCount = tmpRegion.vertexCount;
			if (vCount>0)g_pImmediateContext->Draw(tmpRegion.vertexCount, tmpRegion.startID);
		}

	}
}
