/***********************************************************************

										cpp£ºNoise2DText

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IStaticText::IStaticText()
{
	m_pTextureName = new std::string;

}

NVECTOR2 IStaticText::GetFontSize(UINT fontID)
{
	if (!IsInitialized())return NVECTOR2(0, 0);
	return m_pFatherFontMgr->GetFontSize(fontID);
}

void IStaticText::Destroy()
{
	m_pGraphicObj->SelfDestruction();
};

/************************************************************************
											P R I V A T E
************************************************************************/

void IStaticText::mFunction_InitGraphicObject(UINT pxWidth, UINT pxHeight,NVECTOR4 color,UINT texID)
{

	m_pGraphicObj->AddRectangle(
		NVECTOR2(float(pxWidth) / 2.0f, float(pxHeight) / 2.0f),
		float(pxWidth),
		float(pxHeight),
		color,
		texID
		);

	mStringTextureID = texID;
	*m_pTextColor = color;
	*m_pTextGlowColor = color;
	IBasicContainerInfo::SetWidth(float(pxWidth));
	IBasicContainerInfo::SetHeight(float(pxHeight));
}

void IStaticText::mFunction_UpdateGraphicObject()
{

	//in case other font has been deleted 
	UINT stringTexID = mStringTextureID;

	m_pGraphicObj->SetRectangle(
		0,
		IBasicContainerInfo::GetTopLeft(),
		IBasicContainerInfo::GetBottomRight(),
		NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f),
		stringTexID
		);

	//rectangle depth,used for 2D overlapping
	m_pGraphicObj->SetRectangleDepth(0, mPosZ);
};


