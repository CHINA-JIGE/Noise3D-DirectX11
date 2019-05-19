
/***********************************************************************

			Spherical Harmonic Lighting Common implementation

************************************************************************/

#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

Noise3D::GI::Texture2dSampler_Spherical::Texture2dSampler_Spherical():
	mIsBilinear(false),
	m_pTex(nullptr)
{
}

void Noise3D::GI::Texture2dSampler_Spherical::SetFilterMode(bool isBilinear)
{
	mIsBilinear = isBilinear;
}

void Noise3D::GI::Texture2dSampler_Spherical::SetTexturePtr(Texture2D * pTex)
{
	if (pTex == nullptr)ERROR_MSG("ISphericalFunc: Texture pointer invalid!");
	m_pTex = pTex;
}

Color4f Noise3D::GI::Texture2dSampler_Spherical::Eval(const Vec3 & dir)
{
	Color4f result;
	if (mIsBilinear)
	{
		//pitch, left-handed [-pi/2,pi/2]
		float pitch = 0.0f, yaw = 0.0f;
		Ut::DirectionToYawPitch(dir, yaw, pitch);

		//mapped to [0,1]
		float normalizedU = (yaw / (2.0f * Ut::PI)) + 0.5f;
		float normalizedV = (-pitch / Ut::PI) + 0.5f;

		Color4u c = m_pTex->SamplePixelBilinear(Vec2(normalizedU, normalizedV));
		result = { float(c.r) / 255.0f,float(c.g) / 255.0f,float(c.b) / 255.0f,float(c.a) / 255.0f };
	}
	else
	{
		uint32_t pixelX = 0, pixelY = 0;
		Ut::DirectionToPixelCoord_SphericalMapping(dir, m_pTex->GetWidth(), m_pTex->GetHeight(), pixelX, pixelY);
		Color4u c = m_pTex->GetPixel(pixelX, pixelY);
		result = { float(c.r) / 255.0f,float(c.g) / 255.0f,float(c.b) / 255.0f,float(c.a) / 255.0f };
	}
	return result;
};

//**************************Cube Map Sampler*************************
void Noise3D::GI::CubeMapSampler::SetTexturePtr(TextureCubeMap * pTex)
{
	if (pTex == nullptr)ERROR_MSG("ISphericalFunc: Texture pointer invalid!");
	m_pTex = pTex;
}

Color4f Noise3D::GI::CubeMapSampler::Eval(const Vec3 & dir)
{
	Color4u c = m_pTex->GetPixel(dir,TextureCubeMap::N_TEXTURE_CPU_SAMPLE_MODE::BILINEAR);
	Color4f result = { float(c.r) / 255.0f,float(c.g) / 255.0f,float(c.b) / 255.0f,float(c.a) / 255.0f };
	return result;
}

//**************************Texture2D Sampler*************************
Noise3D::GI::Texture2dSamplerForSHProjection::Texture2dSamplerForSHProjection():
	m_pTex(nullptr)
{
}

void Noise3D::GI::Texture2dSamplerForSHProjection::SetTexturePtr(Texture2D * pTex)
{
	m_pTex = pTex;
}

Color4f Noise3D::GI::Texture2dSamplerForSHProjection::Eval(const Vec3 & dir)
{
	uint32_t pixelX = 0, pixelY = 0;
	Vec3 correctedDir = Vec3(dir.x, -dir.y, dir.z);
	Ut::DirectionToPixelCoord_SphericalMapping(correctedDir, m_pTex->GetWidth(), m_pTex->GetHeight(), pixelX, pixelY);
	Color4u c = m_pTex->GetPixel(pixelX, pixelY);
	Color4f result = { float(c.r) / 255.0f,float(c.g) / 255.0f,float(c.b) / 255.0f,float(c.a) / 255.0f };
	return result;
}
