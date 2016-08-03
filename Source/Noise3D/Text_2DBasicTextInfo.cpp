/***********************************************************************

									cpp£ºNoise2DText

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IBasicTextInfo::IBasicTextInfo()
{
	m_pTextColor = new NVECTOR4(1.0, 1.0f, 0, 1.0f);
	m_pTextGlowColor = new NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
};
