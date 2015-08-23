
/***********************************************************************

								类：NOISE Material Manger

							简述：主要负责管理材质


************************************************************************/

#include "Noise3D.h"

NoiseMaterialManager::NoiseMaterialManager()
{
	m_pFatherScene = NULL;
	m_pMaterialList = new std::vector<N_Material>;
	m_pDefaultMaterial = new N_Material;

	m_pDefaultMaterial->baseColor.mBaseAmbientColor	= NVECTOR3(0, 0, 0);
	m_pDefaultMaterial->baseColor.mBaseDiffuseColor = NVECTOR3(1.0f,1.0f,1.0f);
	m_pDefaultMaterial->baseColor.mBaseSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	m_pDefaultMaterial->baseColor.mSpecularSmoothLevel = 10;
	m_pDefaultMaterial->diffuseMapID		= NOISE_MACRO_INVALID_TEXTURE_ID;
	m_pDefaultMaterial->normalMapID		= NOISE_MACRO_INVALID_TEXTURE_ID;
	m_pDefaultMaterial->specularMapID	= NOISE_MACRO_INVALID_TEXTURE_ID;
	m_pDefaultMaterial->mMatName			= std::string("DEFAULT_MATERIAL");
	m_pMaterialList->push_back(*m_pDefaultMaterial);

};

NoiseMaterialManager::~NoiseMaterialManager()
{

}


UINT NoiseMaterialManager::CreateMaterial(N_Material newMat)
{

	int newMatID = m_pMaterialList->size();
	for (auto m : *m_pMaterialList)
	{
		//if this name has been used ,then we must return a FALSE
		if(m.mMatName == newMat.mMatName)
		{
			DEBUG_MSG1("Error: new material name repeated!!");
			return NOISE_MACRO_DEFAULT_MATERIAL_ID;
		}
	}

	m_pMaterialList->push_back(newMat);
	return newMatID;
}

UINT NoiseMaterialManager::GetIndexByName(std::string * matName)
{
	N_Material tmpMat;

	for (UINT i = 0;i < m_pMaterialList->size();i++)
	{
		tmpMat = m_pMaterialList->at(i);

		//if find specified tex name
		if (tmpMat.mMatName == *matName)
		{
			//return corresponding index
			return i;
		}
	}

	return NOISE_MACRO_DEFAULT_MATERIAL_ID;//Invalid texture Name
}

void NoiseMaterialManager::GetNameByIndex(UINT index, std::string * outMatName)
{
	if (index < m_pMaterialList->size())
	{
		//return the name of corresponding material
		*outMatName = m_pMaterialList->at(index).mMatName;
	}
	
}

UINT NoiseMaterialManager::GetMaterialCount()
{
	return m_pMaterialList->size();
};

