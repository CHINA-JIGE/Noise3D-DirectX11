
/***********************************************************************

							Desc: Text Renderer (D3D)
	
************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IRenderModuleForText::IRenderModuleForText()
{
}

IRenderModuleForText::~IRenderModuleForText()
{
}

void IRenderModuleForText::RenderTexts()
{
	m_pRefRI->SetDepthStencilState(false);
	m_pRefRI->SetRtvAndDsv(IRenderInfrastructure::NOISE_RENDER_STAGE::NORMAL_DRAWING);
	m_pRefRI->SetSampler(IShaderVariableManager::NOISE_SHADER_VAR_SAMPLER::DRAW_2D_SAMPLER, NOISE_SAMPLERMODE::LINEAR);

	//render dynamic/static texts
	mFunction_TextGraphicObj_Render(&mRenderList_TextDynamic);
	mFunction_TextGraphicObj_Render(&mRenderList_TextStatic);
}

void IRenderModuleForText::AddToRenderQueue(IDynamicText * obj)
{
	mFunction_AddToRenderList_Text(obj, &mRenderList_TextDynamic);
}

void IRenderModuleForText::AddToRenderQueue(IStaticText * obj)
{
	mFunction_AddToRenderList_Text(obj, &mRenderList_TextStatic);
}

void IRenderModuleForText::ClearRenderList()
{
	mRenderList_TextDynamic.clear();
	mRenderList_TextStatic.clear();
}

bool IRenderModuleForText::Initialize(IRenderInfrastructure * pRI, IShaderVariableManager * pShaderVarMgr)
{
	m_pRefRI = pRI;
	m_pRefShaderVarMgr = pShaderVarMgr;
	m_pFX_Tech_Solid2D = g_pFX->GetTechniqueByName("DrawSolid2D");
	m_pFX_Tech_DrawText2D = g_pFX->GetTechniqueByName("DrawText2D");
	return true;
}

/***********************************************************************
									P R I V A T E
************************************************************************/
void		IRenderModuleForText::mFunction_AddToRenderList_Text(IBasicTextInfo * pText, std::vector<IBasicTextInfo*>* pList)
{
	pText->mFunction_UpdateGraphicObject();//implemented by derived Text Class
	pList->push_back(pText);
	//Update Data to GPU if data is not up to date , 6 object types for now
	for (UINT i = 0; i <NOISE_GRAPHIC_OBJECT_BUFFER_COUNT; i++)
	{
		if (pText->m_pGraphicObj->mCanUpdateToGpu[i])
		{
			pText->m_pGraphicObj->mFunction_UpdateVerticesToGpu(i);
			pText->m_pGraphicObj->mCanUpdateToGpu[i] = false;
			if (i == NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D)pText->m_pGraphicObj->mFunction_GenerateRectSubsetInfo();
		}
	}
}

void		IRenderModuleForText::mFunction_TextGraphicObj_Update_TextInfo(N_UID uid, ITextureManager* pTexMgr, IBasicTextInfo* pText)
{

	if (pTexMgr->ValidateUID(uid)==true)
	{
		HRESULT hr = S_OK;

		//update text color infos
		float* pColorData = (float*)pText->m_pTextColor;
		m_pRefShaderVarMgr->SetVector4(IShaderVariableManager::NOISE_SHADER_VAR_VECTOR::TEXT_COLOR4, pColorData);

		//update textures
		auto tmp_pSRV = m_pRefRI->GetTextureSRV(pTexMgr,uid);
		m_pRefShaderVarMgr->SetTexture(IShaderVariableManager::NOISE_SHADER_VAR_TEXTURE::COLOR_MAP_2D, tmp_pSRV);
	}
}

void		IRenderModuleForText::mFunction_TextGraphicObj_Render(std::vector<IBasicTextInfo*>* pList)
{
	//prepare to draw , various settings.....
	ID3D11Buffer* tmp_pVB = nullptr;

	for (UINT i = 0;i < pList->size();i++)
	{
		IBasicTextInfo* pText = pList->at(i);
		tmp_pVB = pText->m_pGraphicObj->m_pVB_GPU[NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D];

		//settings
		m_pRefRI->SetInputAssembler(IRenderInfrastructure::NOISE_VERTEX_TYPE::SIMPLE, tmp_pVB, nullptr, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pRefRI->SetRasterState(NOISE_FILLMODE_SOLID, NOISE_CULLMODE_NONE);
		m_pRefRI->SetBlendState(pText->GetBlendMode());

		UINT j = 0, vCount = 0;
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
};

