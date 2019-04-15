
/***********************************************************************

								class£ºMaterial Manger

************************************************************************/

//a DEFAULT MAERIAL will be created with DEFAULT NAME soon as the constructor was invoked
//(because a valid render of a mesh should have some basic default material params)
//and it is invisible in user-created material list ( actually they share the same unorder_map & vector)...

#include "Noise3D.h"
#include "MaterialManager.h"

using namespace Noise3D;

MaterialManager::MaterialManager()
	:IFactoryEx<LambertMaterial, GI::AdvancedGiMaterial>({ 100000, 100000 })
{
	mFunction_CreateDefaultMaterial();
}

MaterialManager::~MaterialManager()
{
	IFactory<LambertMaterial>::DestroyAllObject();
}

LambertMaterial* MaterialManager::CreateLambertMaterial(N_UID matName,const N_LambertMaterialDesc& matDesc)
{
	if (IFactory<LambertMaterial>::FindUid(matName))
	{
		ERROR_MSG("IMaterialManager: material name exist! mat creation failed! name:" + matName);
		return nullptr;
	}

	LambertMaterial* pMat = IFactory<LambertMaterial>::CreateObject(matName);
	pMat->SetDesc(matDesc);
	return pMat;
}

LambertMaterial*		MaterialManager::GetDefaultLambertMaterial()
{
	return IFactory<LambertMaterial>::GetObjectPtr(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
}

GI::AdvancedGiMaterial * Noise3D::MaterialManager::CreateAdvancedMaterial(N_UID matName, const GI::N_AdvancedMatDesc & matDesc)
{
	if (IFactory<GI::AdvancedGiMaterial>::FindUid(matName))
	{
		ERROR_MSG("IMaterialManager: material name exist! mat creation failed! name:" + matName);
		return nullptr;
	}

	GI::AdvancedGiMaterial* pMat = IFactory<GI::AdvancedGiMaterial>::CreateObject(matName);
	pMat->SetDesc(matDesc);
	return pMat;
}

GI::AdvancedGiMaterial * Noise3D::MaterialManager::GetDefaultAdvancedMaterial()
{
	return  IFactory<GI::AdvancedGiMaterial>::GetObjectPtr(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
	;
}

/**********************************************************
							P R I V A T E
*************************************************************/


void MaterialManager::mFunction_CreateDefaultMaterial()
{
	//only with a material can a object be rendered  (even without a texture)
	//thus a default material is needed when an object was rendered with invalid material

	N_LambertMaterialDesc defaultMatDesc;
	LambertMaterial* pLambertMat = IFactory<LambertMaterial>::CreateObject(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
	pLambertMat->SetDesc(defaultMatDesc);

	//------------------------------
	GI::N_AdvancedMatDesc giMatDesc;
	GI::AdvancedGiMaterial* pGiMat = IFactory<GI::AdvancedGiMaterial>::CreateObject(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
	pGiMat->SetDesc(giMatDesc);
}
