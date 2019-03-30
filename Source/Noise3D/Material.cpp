
/***********************************************************************

							desc£ºMaterial Interfaces

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

Material::Material()
{
}

Material::~Material()
{
}

void Material::SetAmbientColor(const Vec3 & color)
{
	mMatDesc.ambientColor = Clamp(color, Vec3(0, 0, 0), Vec3(1.0f, 1.0f, 1.0f));
}

void Material::SetDiffuseColor(const Vec3 & color)
{
	mMatDesc.diffuseColor = Clamp(color, Vec3(0, 0, 0), Vec3(1.0f, 1.0f, 1.0f));
}

void Material::SetSpecularColor(const Vec3 & color)
{
	mMatDesc.specularColor = Clamp(color, Vec3(0, 0, 0), Vec3(1.0f, 1.0f, 1.0f));
}

void Material::SetSpecularSmoothLevel(int level)
{
	mMatDesc.specularSmoothLevel = Clamp(level, 0, 100);
}

void Material::SetNormalMapBumpIntensity(float normalized_intensity)
{
	mMatDesc.normalMapBumpIntensity = Clamp(normalized_intensity, 0.0f, 1.0f);
}

void Material::SetEnvironmentMappingTransparency(float transparency)
{
	mMatDesc.environmentMapTransparency = Clamp(transparency, 0.0f, 1.0f);
}

void Material::SetTransparency(float transparency)
{
	mMatDesc.transparency = Clamp(transparency, 0.0f, 1.0f);
}

void Material::SetDiffuseMap(const N_UID & diffMapName)
{
	mMatDesc.diffuseMapName = diffMapName;
}

void Material::SetNormalMap(const N_UID & normMapName)
{
	mMatDesc.normalMapName = normMapName;
}

void Material::SetSpecularMap(const N_UID & specMapName)
{
	mMatDesc.specularMapName = specMapName;
}

void Material::SetEnvMap(const N_UID & environmentMapName)
{
	mMatDesc.environmentMapName = environmentMapName;
}

void Material::SetDesc(const N_MaterialDesc & desc)
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

void Material::GetDesc(N_MaterialDesc & outDesc)
{
	outDesc = mMatDesc;
}
