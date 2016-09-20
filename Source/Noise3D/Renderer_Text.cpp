
/***********************************************************************

							Desc: Text Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

const UINT c_VBstride_Default = sizeof(N_DefaultVertex);		//VertexBuffer的每个元素的字节跨度
const UINT c_VBstride_Simple = sizeof(N_SimpleVertex);
const UINT c_VBoffset = 0;				//VertexBuffer顶点序号偏移 因为从头开始所以offset是0


void IRenderer::RenderTexts()
{

	//CLEAR DEPTH!! to implement component overlapping
	g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);
	mFunction_SetBlendState(m_BlendMode);
	m_pFX_SamplerState_Default->SetSampler(0, m_pSamplerState_FilterAnis);
	g_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState_DisableDepthTest, 0xffffffff);

	//render TEXT
	mFunction_TextGraphicObj_Render(m_pRenderList_TextDynamic);
	mFunction_TextGraphicObj_Render(m_pRenderList_TextStatic);
}


/***********************************************************************
									P R I V A T E
************************************************************************/

void		IRenderer::mFunction_TextGraphicObj_Update_TextInfo(N_UID uid, ITextureManager* pTexMgr, N_CbDrawText2D& cbText)
{
	//Get Shader Resource View
	ID3D11ShaderResourceView* tmp_pSRV = NULL;

	//......
	//texID = mFunction_ValidateTextureID_UsingTexMgr(texID, NOISE_TEXTURE_TYPE_TEXT,NOISE_TEXTURE_ACCESS_PERMISSION_FONTMGR);

	if (pTexMgr->ValidateUID(uid)==TRUE)
	{
		HRESULT hr = S_OK;
		m_CbDrawText2D = (N_CbDrawText2D)cbText;
		//update Constant buffer
		m_pFX_CbDrawText2D->SetRawValue(&m_CbDrawText2D, 0, sizeof(m_CbDrawText2D));

		//update textures
		tmp_pSRV = pTexMgr->GetObjectPtr(uid)->m_pSRV;
		m_pFX2D_Texture_Diffuse->SetResource(tmp_pSRV);
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
		tmp_pVB = pList->at(i)->m_pGraphicObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];
		g_pImmediateContext->IASetInputLayout(g_pVertexLayout_Simple);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &tmp_pVB, &c_VBstride_Simple, &c_VBoffset);
		g_pImmediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
		g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//设置fillmode和cullmode
		mFunction_SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);

		UINT j = 0, vCount = 0;
		//traverse all region list , to decide use which tech to draw (textured or not)

		//---------------------draw rectangles---------------------
		for (auto tmpRegion : *(pList->at(i)->m_pGraphicObj->m_pRectSubsetInfoList))
		{
			IFontManager* pFontMgr = GetScene()->GetFontMgr();

			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//THIS TEXTURE MANAGER BELONGS TO FONT MGR,not the same as scene tex mgr
			BOOL texUidValid = pFontMgr->m_pTexMgr->ValidateUID(tmpRegion.texName, NOISE_TEXTURE_TYPE_COMMON);

			//if current Rectangle disable Texture ,then draw in a solid way
			if (texUidValid==FALSE)
			{
				m_pFX_Tech_Solid2D->GetPassByIndex(0)->Apply(0, g_pImmediateContext);
			}
			else
			{
				//fill cb struct......
				N_CbDrawText2D tmpCbText;
				ZeroMemory(&tmpCbText, sizeof(tmpCbText));
				tmpCbText.mTextColor = *(pList->at(i)->m_pTextColor);
				tmpCbText.mTextGlowColor = *(pList->at(i)->m_pTextGlowColor);

				//update colors of text(cb & srv)
				mFunction_TextGraphicObj_Update_TextInfo(tmpRegion.texName, pFontMgr->m_pTexMgr, tmpCbText);
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
