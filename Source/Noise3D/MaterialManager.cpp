
/***********************************************************************

								class£ºMaterial Manger

************************************************************************/

//a DEFAULT MAERIAL will be created with DEFAULT NAME soon as the constructor was invoked
//(because a valid render of a mesh should have some basic default material params)
//and it is invisible in user-created material list ( actually they share the same unorder_map & vector)...

#include "Noise3D.h"

using namespace Noise3D;

MaterialManager::MaterialManager()
	:IFactory<Material>(100000)
{
	mFunction_CreateDefaultMaterial();
}

MaterialManager::~MaterialManager()
{
	IFactory<Material>::DestroyAllObject();
}

Material* MaterialManager::CreateMaterial(N_UID matName,const N_MaterialDesc& matDesc)
{
	if (IFactory<Material>::FindUid(matName))
	{
		ERROR_MSG("IMaterialManager: material name exist! mat creation failed! name:" + matName);
		return nullptr;
	}

	Material* pMat = IFactory<Material>::CreateObject(matName);
	pMat->SetDesc(matDesc);
	return pMat;
}

Material*		MaterialManager::GetDefaultMaterial()
{
	return IFactory<Material>::GetObjectPtr(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
}

/**********************************************************
							P R I V A T E
*************************************************************/


void MaterialManager::mFunction_CreateDefaultMaterial()
{
	//only with a material can a object be rendered  (even without a texture)
	//thus a default material is needed when an object was rendered with invalid material

	N_MaterialDesc defaultMatDesc;
	defaultMatDesc.ambientColor = Vec3(0.0f, 0.0f, 0.0f);
	defaultMatDesc.diffuseColor = Vec3(0.1f, 0.1f, 0.1f);
	defaultMatDesc.specularColor = Vec3(1.0f, 1.0f, 1.0f);
	defaultMatDesc.environmentMapTransparency = 0.0f;
	defaultMatDesc.normalMapBumpIntensity = 0.1f;
	defaultMatDesc.specularSmoothLevel = 10;
	defaultMatDesc.diffuseMapName = "";
	defaultMatDesc.specularMapName = "";
	defaultMatDesc.environmentMapName = "";
	defaultMatDesc.normalMapName = "";

	
	Material* pMat = IFactory<Material>::CreateObject(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
	pMat->SetDesc(defaultMatDesc);
}
