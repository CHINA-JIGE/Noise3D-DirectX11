
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
	:IFactoryEx<LambertMaterial, GI::PbrtMaterial>({ 100000, 100000 })
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

GI::PbrtMaterial * Noise3D::MaterialManager::CreateAdvancedMaterial(N_UID matName, const GI::N_PbrtMatDesc & matDesc)
{
	if (IFactory<GI::PbrtMaterial>::FindUid(matName))
	{
		ERROR_MSG("IMaterialManager: material name exist! mat creation failed! name:" + matName);
		return nullptr;
	}

	GI::PbrtMaterial* pMat = IFactory<GI::PbrtMaterial>::CreateObject(matName);
	pMat->SetDesc(matDesc);
	return pMat;
}

GI::PbrtMaterial * Noise3D::MaterialManager::GetDefaultAdvancedMaterial()
{
	return  IFactory<GI::PbrtMaterial>::GetObjectPtr(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
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
	GI::N_PbrtMatDesc giMatDesc;
	GI::PbrtMaterial* pGiMat = IFactory<GI::PbrtMaterial>::CreateObject(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
	pGiMat->SetDesc(giMatDesc);
}
