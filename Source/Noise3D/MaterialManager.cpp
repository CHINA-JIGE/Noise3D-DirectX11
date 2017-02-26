
/***********************************************************************

								class£ºNOISE Material Manger

							desc£ºmanager material

************************************************************************/

//a DEFAULT MAERIAL will be created with DEFAULT NAME soon as the constructor was invoked
//(because a valid render of a mesh should have some basic default material params)
//and it is invisible in user-created material list ( actually they share the same unorder_map & vector)...

#include "Noise3D.h"

using namespace Noise3D;

 const N_UID IMaterialManager::mDefaultMatName = "N3d_DeFaUlT_MaT";

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
	if (matName == mDefaultMatName)
	{
		ERROR_MSG("CreateMaterial: uid occupied (by default material, how lucky you are- -.)");
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
	return IFactory<IMaterial>::GetObjectPtr(mDefaultMatName);
}

IMaterial* IMaterialManager::GetMaterial(N_UID matName)
{
	if (matName == mDefaultMatName)return nullptr;
	return IFactory<IMaterial>::GetObjectPtr(matName);
};

BOOL IMaterialManager::DeleteMaterial(N_UID matName)
{
	if (matName == mDefaultMatName)
	{
		ERROR_MSG("DeleteMaterial: default material can't be deleted...(how lucky you are- -.)");
		return FALSE;
	}
	return IFactory<IMaterial>::DestroyObject(matName);
}

void IMaterialManager::DeleteAllMaterial()
{
	IFactory<IMaterial>::DestroyAllObject();
	//we delete user-created material, not the internal default one
	mFunction_CreateDefaultMaterial();
}

inline BOOL IMaterialManager::ValidateUID(N_UID matName)
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
	defaultMatDesc.mAmbientColor = NVECTOR3(0.0f, 0.0f, 0.0f);
	defaultMatDesc.mDiffuseColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	defaultMatDesc.mSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	defaultMatDesc.mEnvironmentMapTransparency = 0.0f;
	defaultMatDesc.mNormalMapBumpIntensity = 0.1f;
	defaultMatDesc.mSpecularSmoothLevel = 10;
	defaultMatDesc.diffuseMapName = "";
	defaultMatDesc.specularMapName = "";
	defaultMatDesc.environmentMapName = "";
	defaultMatDesc.normalMapName = "";

	
	IMaterial* pMat = IFactory<IMaterial>::CreateObject(mDefaultMatName);
	pMat->SetDesc(defaultMatDesc);
}
