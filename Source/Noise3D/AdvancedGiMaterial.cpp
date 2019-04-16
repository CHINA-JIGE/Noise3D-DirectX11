
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
	r = Ut::Clamp(r, 0.0f, 1.0f);
	mMatDesc.roughness = r;
}

void Noise3D::GI::AdvancedGiMaterial::SetOpacity(float o)
{
	o= Ut::Clamp(o, 0.0f, 1.0f);
	mMatDesc.opacity = o;
}

void Noise3D::GI::AdvancedGiMaterial::SetMetallicity(float m)
{
	m = Ut::Clamp(m, 0.0f, 1.0f);
	mMatDesc.metallicity = m;
}

void Noise3D::GI::AdvancedGiMaterial::SetEmission(Vec3 hdrEmission)
{
	mMatDesc.emission = hdrEmission;
}

void Noise3D::GI::AdvancedGiMaterial::SetMetalF0(Vec3 F0)
{
	mMatDesc.metal_F0 = Ut::Clamp(F0,Vec3(0,0,0),Vec3(1.0f,1.0f,1.0f));
}

void Noise3D::GI::AdvancedGiMaterial::SetRefractiveIndex(float ior)
{
	if (ior <= 1.0f)ior = 1.001f;
	mMatDesc.ior = ior;
}

bool Noise3D::GI::AdvancedGiMaterial::IsTransmissionEnabled()
{
	return mMatDesc.opacity != 1.0f;
}

bool Noise3D::GI::AdvancedGiMaterial::IsEmissionEnabled()
{
	return mMatDesc.emission== Vec3(0,0,0);
}

void Noise3D::GI::AdvancedGiMaterial::SetDesc(const N_AdvancedMatDesc & desc)
{
	mMatDesc = desc;
}

const GI::N_AdvancedMatDesc & Noise3D::GI::AdvancedGiMaterial::GetDesc()
{
	return mMatDesc;
}

void Noise3D::GI::AdvancedGiMaterial::Preset_PerfectGlass(float ior)
{
	mMatDesc.albedo = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	mMatDesc.emission = Vec3(0, 0, 0);
	mMatDesc.ior = Ut::Clamp(ior,1.0f,std::numeric_limits<float>::infinity());
	mMatDesc.metallicity = 0.0f;
	float F0 = mFunc_IorToF0(mMatDesc.ior);
	mMatDesc.metal_F0 = Vec3(F0, F0, F0);
	mMatDesc.opacity = 0.0f;
	mMatDesc.roughness = 0.0f;
}

float Noise3D::GI::AdvancedGiMaterial::mFunc_IorToF0(float ior)
{
	float x = (ior - 1.0f) / (ior + 1.0f);
	return x*x;
}

//---------------------------------------------

Noise3D::GI::IAdvancedGiMaterialOwner::IAdvancedGiMaterialOwner():
	m_pGiMat(nullptr)
{
	m_pGiMat = Noise3D::GetScene()->GetMaterialMgr()->GetDefaultAdvancedMaterial();
}

Noise3D::GI::IAdvancedGiMaterialOwner::~IAdvancedGiMaterialOwner()
{
}

void Noise3D::GI::IAdvancedGiMaterialOwner::SetGiMaterial(GI::AdvancedGiMaterial * pMat)
{
	m_pGiMat = pMat;
}

GI::AdvancedGiMaterial * Noise3D::GI::IAdvancedGiMaterialOwner::GetGiMaterial()
{
	return m_pGiMat;
}
