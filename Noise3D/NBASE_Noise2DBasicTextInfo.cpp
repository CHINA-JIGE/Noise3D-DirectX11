/***********************************************************************

									cpp£ºNoise2DText

************************************************************************/
#include "Noise3D.h"

Noise2DBasicTextInfo::Noise2DBasicTextInfo()
{
	m_pTextColor = new NVECTOR4(1.0, 1.0f, 0, 1.0f);
	m_pTextGlowColor = new NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pGraphicObj = new NoiseGraphicObject;
	m_pFatherFontMgr = nullptr;
};