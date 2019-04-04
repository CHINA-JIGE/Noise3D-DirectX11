
/***********************************************************************

							desc£ºMaterial Interfaces

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

LambertMaterial::LambertMaterial()
{
}

LambertMaterial::~LambertMaterial()
{
}

void LambertMaterial::SetAmbientColor(const Vec3 & color)
{
	mMatDesc.ambientColor = Clamp(color, Vec3(0, 0, 0), Vec3(1.0f, 1.0f, 1.0f));
}

void LambertMaterial::SetDiffuseColor(const Vec3 & color)
{
	mMatDesc.diffuseColor = Clamp(color, Vec3(0, 0, 0), Vec3(1.0f, 1.0f, 1.0f));
}

void LambertMaterial::SetSpecularColor(const Vec3 & color)
{
	mMatDesc.specularColor = Clamp(color, Vec3(0, 0, 0), Vec3(1.0f, 1.0f, 1.0f));
}

void LambertMaterial::SetSpecularSmoothLevel(int level)
{
	mMatDesc.specularSmoothLevel = Clamp(level, 0, 100);
}

void LambertMaterial::SetNormalMapBumpIntensity(float normalized_intensity)
{
	mMatDesc.normalMapBumpIntensity = Clamp(normalized_intensity, 0.0f, 1.0f);
}

void LambertMaterial::SetEnvironmentMappingTransparency(float transparency)
{
	mMatDesc.environmentMapTransparency = Clamp(transparency, 0.0f, 1.0f);
}

void LambertMaterial::SetTransparency(float transparency)
{
	mMatDesc.transparency = Clamp(transparency, 0.0f, 1.0f);
}

void LambertMaterial::SetDiffuseMap(const N_UID & diffMapName)
{
	mMatDesc.diffuseMapName = diffMapName;
}

void LambertMaterial::SetNormalMap(const N_UID & normMapName)
{
	mMatDesc.normalMapName = normMapName;
}

void LambertMaterial::SetSpecularMap(const N_UID & specMapName)
{
	mMatDesc.specularMapName = specMapName;
}

void LambertMaterial::SetEnvMap(const N_UID & environmentMapName)
{
	mMatDesc.environmentMapName = environmentMapName;
}

void LambertMaterial::SetDesc(const N_LambertMaterialDesc & desc)
{
	SetAmbientColor(desc.ambientColor);
	SetDiffuseColor(desc.diffuseColor);
	SetSpecularColor(desc.specularColor);
	SetSpecularSmoothLevel(desc.specularSmoothLevel);
	SetNormalMapBumpIntensity(desc.normalMapBumpIntensity);
	SetEnvironmentMappingTransparency(desc.environmentMapTransparency);
	SetTransparency(desc.transparency);

	SetDiffuseMap(desc.diffuseMapName);
	SetNormalMap(desc.normalMapName);
	SetSpecularMap(desc.specularMapName);
	SetEnvMap(desc.environmentMapName);
}

void LambertMaterial::GetDesc(N_LambertMaterialDesc & outDesc)
{
	outDesc = mMatDesc;
}
