/***********************************************************************

										cpp£ºNoise2DText

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IStaticText::IStaticText()
{
}

IStaticText::~IStaticText()
{
}

NVECTOR2 IStaticText::GetFontSize()
{
	//this is static text, font size is fixed when created. font name was recorded,
	//thus we can query font size from FontMgr
	IFontManager* pFontMgr = GetScene()->GetFontMgr();
	return pFontMgr->GetFontSize(mFontName);
}


/************************************************************************
											P R I V A T E
************************************************************************/

void IStaticText::mFunction_InitGraphicObject(IGraphicObject* pCreatedObj,UINT pxWidth, UINT pxHeight,NVECTOR4 color,N_UID texName)
{

	m_pGraphicObj = pCreatedObj;
	mTextureName = texName;

	m_pGraphicObj->AddRectangle(
		NVECTOR2(float(pxWidth) / 2.0f, float(pxHeight) / 2.0f),
		float(pxWidth),
		float(pxHeight),
		color,
		texName
		);

	*m_pTextColor = color;
	*m_pTextGlowColor = color;
	IBasicContainerInfo::SetWidth(float(pxWidth));
	IBasicContainerInfo::SetHeight(float(pxHeight));
}

void IStaticText::mFunction_UpdateGraphicObject()
{
	//in case other font has been deleted 

	m_pGraphicObj->SetRectangle(
		0,
		IBasicContainerInfo::GetTopLeft(),
		IBasicContainerInfo::GetBottomRight(),
		NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f),
		mTextureName
		);

	//rectangle depth,used for 2D overlapping
	m_pGraphicObj->SetRectangleDepth(0, mPosZ);
};


