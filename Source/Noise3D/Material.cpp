
/***********************************************************************

							desc£ºMaterial Interfaces

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IMaterial::IMaterial()
{
}

IMaterial::~IMaterial()
{
}

void IMaterial::SetAmbientColor(const NVECTOR3 & color)
{
	mMatDesc.ambientColor = Clamp(color, NVECTOR3(0, 0, 0), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IMaterial::SetDiffuseColor(const NVECTOR3 & color)
{
	mMatDesc.diffuseColor = Clamp(color, NVECTOR3(0, 0, 0), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IMaterial::SetSpecularColor(const NVECTOR3 & color)
{
	mMatDesc.specularColor = Clamp(color, NVECTOR3(0, 0, 0), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IMaterial::SetSpecularSmoothLevel(int level)
{
	mMatDesc.specularSmoothLevel = Clamp(level, 0, 100);
}

void IMaterial::SetNormalMapBumpIntensity(float normalized_intensity)
{
	mMatDesc.normalMapBumpIntensity = Clamp(normalized_intensity, 0.0f, 1.0f);
}

void IMaterial::SetEnvironmentMappingTransparency(float transparency)
{
	mMatDesc.environmentMapTransparency = Clamp(transparency, 0.0f, 1.0f);
}

void IMaterial::SetTransparency(float transparency)
{
	mMatDesc.transparency = Clamp(transparency, 0.0f, 1.0f);
}

void IMaterial::SetDiffuseMap(const N_UID & diffMapName)
{
	mMatDesc.diffuseMapName = diffMapName;
}

void IMaterial::SetNormalMap(const N_UID & normMapName)
{
	mMatDesc.normalMapName = normMapName;
}

void IMaterial::SetSpecularMap(const N_UID & specMapName)
{
	mMatDesc.specularMapName = specMapName;
}

void IMaterial::SetEnvMap(const N_UID & environmentMapName)
{
	mMatDesc.environmentMapName = environmentMapName;
}

void IMaterial::SetDesc(const N_MaterialDesc & desc)
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

void IMaterial::GetDesc(N_MaterialDesc & outDesc)
{
	outDesc = mMatDesc;
}
