
/***********************************************************************

			Spherical Harmonic Lighting Common implementation

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void Noise3D::GI::ISphericalFunc_Texture2dSampler::SetTexturePtr(Texture2D * pTex)
{
	if (pTex == nullptr)ERROR_MSG("ISphericalFunc: Texture pointer invalid!");
	m_pTex = pTex;
}

Color4f Noise3D::GI::ISphericalFunc_Texture2dSampler::Eval(const Vec3 & dir)
{
	uint32_t pixelX = 0, pixelY = 0;
	Ut::DirectionToPixelCoord_SphericalMapping(dir, m_pTex->GetWidth(), m_pTex->GetHeight(), pixelX, pixelY);
	NColor4u c=  m_pTex->GetPixel(pixelX, pixelY);
	Color4f result = { float(c.r) / 255.0f,float(c.g) / 255.0f,float(c.b) / 255.0f,float(c.a) / 255.0f };
	return result;
};


//**************************Cube Map Sampler*************************
void Noise3D::GI::ISphericalFunc_CubeMapSampler::SetTexturePtr(TextureCubeMap * pTex)
{
	if (pTex == nullptr)ERROR_MSG("ISphericalFunc: Texture pointer invalid!");
	m_pTex = pTex;
}

Color4f Noise3D::GI::ISphericalFunc_CubeMapSampler::Eval(const Vec3 & dir)
{
	NColor4u c = m_pTex->GetPixel(dir,TextureCubeMap::N_TEXTURE_CPU_SAMPLE_MODE::BILINEAR);
	Color4f result = { float(c.r) / 255.0f,float(c.g) / 255.0f,float(c.b) / 255.0f,float(c.a) / 255.0f };
	return result;
}
