
/***********************************************************************

							desc£ºAdvanced Material for Path Tracer 

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

void Noise3D::GI::AdvancedGiMaterial::SetAlbedo(Color4f albedo)
{
	mMatDesc.albedo = albedo;
}

void Noise3D::GI::AdvancedGiMaterial::SetRoughness(float r)
{
	mMatDesc.roughness = r;
}

void Noise3D::GI::AdvancedGiMaterial::SetOpacity(float o)
{
	mMatDesc.opacity = o;
}

void Noise3D::GI::AdvancedGiMaterial::SetMetallicity(float m)
{
	mMatDesc.metallicity = m;
}

void Noise3D::GI::AdvancedGiMaterial::SetEmission(Vec3 hdrEmission)
{
	mMatDesc.emission = hdrEmission;
}

bool Noise3D::GI::AdvancedGiMaterial::IsTransmissionEnabled()
{
	return mMatDesc.opacity != 1.0f;
}

bool Noise3D::GI::AdvancedGiMaterial::IsEmissionEnabled()
{
	return mMatDesc.emission==Vec3(0,0,0);
}

void Noise3D::GI::AdvancedGiMaterial::SetDesc(const N_AdvancedMatDesc & desc)
{
	mMatDesc = desc;
}

GI::N_AdvancedMatDesc & Noise3D::GI::AdvancedGiMaterial::GetDesc()
{
	return mMatDesc;
}
