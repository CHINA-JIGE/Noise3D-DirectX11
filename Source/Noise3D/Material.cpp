
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

void IMaterial::SetBaseAmbientColor(const NVECTOR3 & color)
{
	mMatDesc.mAmbientColor = Clamp(color, NVECTOR3(0, 0, 0), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IMaterial::SetBaseDiffuseColor(const NVECTOR3 & color)
{
	mMatDesc.mDiffuseColor = Clamp(color, NVECTOR3(0, 0, 0), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IMaterial::SetBaseSpecularColor(const NVECTOR3 & color)
{
	mMatDesc.mSpecularColor = Clamp(color, NVECTOR3(0, 0, 0), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IMaterial::SetSpecularSmoothLevel(int level)
{
	mMatDesc.mSpecularSmoothLevel = Clamp(level, 0, 100);
}

void IMaterial::SetNormalMapBumpIntensity(float normalized_intensity)
{
	mMatDesc.mNormalMapBumpIntensity = Clamp(normalized_intensity, 0.0f, 1.0f);
}

void IMaterial::SetEnvironmentMappingTransparency(float transparency)
{
	mMatDesc.mEnvironmentMapTransparency = Clamp(transparency, 0.0f, 1.0f);
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
	SetBaseAmbientColor(desc.mAmbientColor);
	SetBaseDiffuseColor(desc.mDiffuseColor);
	SetBaseSpecularColor(desc.mSpecularColor);
	SetSpecularSmoothLevel(desc.mSpecularSmoothLevel);
	SetNormalMapBumpIntensity(desc.mNormalMapBumpIntensity);
	SetEnvironmentMappingTransparency(desc.mEnvironmentMapTransparency);

	SetDiffuseMap(desc.diffuseMapName);
	SetNormalMap(desc.normalMapName);
	SetSpecularMap(desc.specularMapName);
	SetEnvMap(desc.environmentMapName);
}

void IMaterial::GetDesc(N_MaterialDesc & outDesc)
{
	outDesc = mMatDesc;
}
