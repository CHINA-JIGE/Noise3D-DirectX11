
/***********************************************************************

			Spherical Harmonic Lighting Common implementation

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void Noise3D::GI::ISphericalMappingTextureSampler::SetTexturePtr(Texture2D * pTex)
{
	if (pTex == nullptr)ERROR_MSG("ISphericalFunc: Texture pointer invalid!");
	m_pTex = pTex;
}

NVECTOR3 Noise3D::GI::ISphericalMappingTextureSampler::Eval(const NVECTOR3 & dir)
{
	//pitch, left-handed [-pi/2,pi/2]
	float pitch = atan2(dir.y, sqrtf(dir.x*dir.x + dir.z*dir.z));

	//yaw, start from z, left-handed
	float yaw = atan2(dir.x, dir.z);

	//mapped to [0,1]
	float normalizedU= (yaw / (2.0f * Ut::PI) + 0.5f);
	float normalizedV = (pitch /  Ut::PI) + 0.5f;
	uint32_t width = m_pTex->GetWidth();
	uint32_t height = m_pTex->GetHeight();
	uint32_t x = uint32_t(float(width) * normalizedU);
	uint32_t y = uint32_t(float(height) * normalizedV);
	if (x == width) x = width - 1;
	if (y == height) y = height - 1;

	NColor4u c=  m_pTex->GetPixel(x, y);
	NVECTOR3 result = { c.r / 256.0f, c.g / 256.0f, c.b / 256.0f };
	return result;
};