
/***********************************************************************

								class£ºNOISE Material Manger

							desc£ºmanager material

************************************************************************/

//a DEFAULT MAERIAL will be created with DEFAULT NAME soon as the constructor was invoked
//(because a valid render of a mesh should have some basic default material params)
//and it is invisible in user-created material list ( actually they share the same unorder_map & vector)...

#include "Noise3D.h"

using namespace Noise3D;

IMaterialManager::IMaterialManager()
	:IFactory<IMaterial>(100000)
{
	mFunction_CreateDefaultMaterial();
}

IMaterialManager::~IMaterialManager()
{
	IFactory<IMaterial>::DestroyAllObject();
}

IMaterial* IMaterialManager::CreateMaterial(N_UID matName,const N_MaterialDesc& matDesc)
{
	if (IFactory<IMaterial>::FindUid(matName))
	{
		ERROR_MSG("IMaterialManager: material name exist! mat creation failed! name:" + matName);
		return nullptr;
	}

	IMaterial* pMat = IFactory<IMaterial>::CreateObject(matName);
	pMat->SetDesc(matDesc);
	return pMat;
}


UINT IMaterialManager::GetMaterialCount()
{
	//minus 1 means ruling out default mat
	return  IFactory<IMaterial>::GetObjectCount() - 1;
}

IMaterial*		IMaterialManager::GetDefaultMaterial()
{
	return IFactory<IMaterial>::GetObjectPtr(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
}

IMaterial* IMaterialManager::GetMaterial(N_UID matName)
{
	return IFactory<IMaterial>::GetObjectPtr(matName);
};

bool IMaterialManager::DeleteMaterial(N_UID matName)
{
	if (matName == NOISE_MACRO_DEFAULT_MATERIAL_NAME)
	{
		ERROR_MSG("DeleteMaterial: default material can't be deleted...(how lucky you are- -.)");
		return false;
	}
	return IFactory<IMaterial>::DestroyObject(matName);
}

void IMaterialManager::DeleteAllMaterial()
{
	IFactory<IMaterial>::DestroyAllObject();
	//we delete user-created material, not the internal default one
	mFunction_CreateDefaultMaterial();
}

bool IMaterialManager::ValidateUID(N_UID matName)
{
	return IFactory<IMaterial>::FindUid(matName);
};

/**********************************************************
							P R I V A T E
*************************************************************/


void IMaterialManager::mFunction_CreateDefaultMaterial()
{
	//only with a material can a object be rendered  (even without a texture)
	//thus a default material is needed when an object was rendered with invalid material

	N_MaterialDesc defaultMatDesc;
	defaultMatDesc.ambientColor = NVECTOR3(0.0f, 0.0f, 0.0f);
	defaultMatDesc.diffuseColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	defaultMatDesc.specularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	defaultMatDesc.environmentMapTransparency = 0.0f;
	defaultMatDesc.normalMapBumpIntensity = 0.1f;
	defaultMatDesc.specularSmoothLevel = 10;
	defaultMatDesc.diffuseMapName = "";
	defaultMatDesc.specularMapName = "";
	defaultMatDesc.environmentMapName = "";
	defaultMatDesc.normalMapName = "";

	
	IMaterial* pMat = IFactory<IMaterial>::CreateObject(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
	pMat->SetDesc(defaultMatDesc);
}
