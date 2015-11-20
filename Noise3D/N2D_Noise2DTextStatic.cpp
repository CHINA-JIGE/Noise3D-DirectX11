/***********************************************************************

										cpp£ºNoise2DText

************************************************************************/
#include "Noise3D.h"

Noise2DTextStatic::Noise2DTextStatic()
{
	mIsInitialized = FALSE;//set TRUE when created
	m_pTextureName = new std::string;
	m_pTextColor = new NVECTOR4(0, 0, 0, 1.0f);
	m_pTextGlowColor = new NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pGraphicObj = new NoiseGraphicObject;

}

void Noise2DTextStatic::SelfDestruction()
{
	m_pGraphicObj->SelfDestruction();
	mIsInitialized = FALSE;
};

void Noise2DTextStatic::SetTextColor(NVECTOR4 color)
{
	*m_pTextColor = color;
};

void Noise2DTextStatic::SetTextGlowColor(NVECTOR4 color)
{
	*m_pTextGlowColor = color;
};


/************************************************************************
											P R I V A T E
************************************************************************/

void Noise2DTextStatic::mFunction_InitGraphicObject(UINT pxWidth, UINT pxHeight,NVECTOR4 color,UINT texID)
{

	m_pGraphicObj->AddRectangle(
		NVECTOR2(float(pxWidth) / 2.0f, float(pxHeight) / 2.0f),
		float(pxWidth),
		float(pxHeight),
		color,
		texID
		);

	mTextureID = texID;
	*m_pTextColor = color;
	*m_pTextGlowColor = color;
	Noise2DBasicContainerInfo::SetWidth(float(pxWidth));
	Noise2DBasicContainerInfo::SetHeight(float(pxHeight));
}

void Noise2DTextStatic::mFunction_UpdateGraphicObject()
{
	if (!mIsInitialized)return;

	//in case other font has been deleted 
	UINT stringTexID = mTextureID;

	m_pGraphicObj->SetRectangle(
		0,
		Noise2DBasicContainerInfo::GetTopLeft(),
		Noise2DBasicContainerInfo::GetBottomRight(),
		NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f),
		stringTexID
		);
};


