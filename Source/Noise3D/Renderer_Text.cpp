
/***********************************************************************

							Desc: Text Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void IRenderer::RenderTexts()
{

	//CLEAR DEPTH!! to implement component overlapping
	g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//render TEXT
	mFunction_TextGraphicObj_Render(m_pRenderList_TextDynamic);
	mFunction_TextGraphicObj_Render(m_pRenderList_TextStatic);
}

/***********************************************************************
									P R I V A T E
************************************************************************/

void		IRenderer::mFunction_TextGraphicObj_Update_TextInfo(N_UID uid, ITextureManager* pTexMgr, IBasicTextInfo* pText)
{

	if (pTexMgr->ValidateUID(uid)==true)
	{
		HRESULT hr = S_OK;

		//update text color infos
		float* pColorData = (float*)pText->m_pTextColor;
		IShaderVariableManager::m_pFxVector4_TextColor->SetFloatVector(pColorData);

		//update textures
		ID3D11ShaderResourceView* tmp_pSRV = pTexMgr->GetObjectPtr(uid)->m_pSRV;
		IShaderVariableManager::m_pFxTexture_ColorMap2D->SetResource(tmp_pSRV);
	}
}

void		IRenderer::mFunction_TextGraphicObj_Render(std::vector<IBasicTextInfo*>* pList)
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = NULL;

	//I didn't use template because this function is type-dependent
	//i dont want it to be type-unsafe


	for (UINT i = 0;i < pList->size();i++)
	{
		IBasicTextInfo* pText = pList->at(i);
		tmp_pVB = pText->m_pGraphicObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];

		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &g_cVBstride_Simple, &g_cVBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//set fillmode & cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

		//set blend state
		mFunction_SetBlendState(pText->GetBlendMode());

		//set samplerState
		m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterLinear);

		//set depth/Stencil State
		g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_EnableDepthTest, 0xffffffff);


		UINT j = 0, vCount = 0;
		//traverse all region list , to decide use which tech to draw (textured or not)

		//---------------------draw rectangles---------------------
		for (auto tmpRegion : *(pList->at(i)->m_pGraphicObj->m_pRectSubsetInfoList))
		{
			IFontManager* pFontMgr = GetScene()->GetFontMgr();

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//THIS TEXTURE MANAGER BELONGS TO FONT MGR,not the same as scene tex mgr
			bool texUidValid = pFontMgr->m_pTexMgr->ValidateUID(tmpRegion.texName, NOISE_TEXTURE_TYPE_COMMON);

			//if current Rectangle disable Texture ,then draw in a solid way
			if (texUidValid== false)
			{
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//update colors of text(colors & srv)
				mFunction_TextGraphicObj_Update_TextInfo(tmpRegion.texName, pFontMgr->m_pTexMgr, pList->at(i));
				
				//issue draw call
				m_pFX_Tech_DrawText2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}

			//draw 
			vCount = tmpRegion.vertexCount;
			if (vCount>0)g_pImmediateContext->Draw(tmpRegion.vertexCount, tmpRegion.startID);
		}

	}

	//clear render list
	pList->clear();
};
