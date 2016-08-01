
/***********************************************************************

								类：NOISE Material Manger

							简述：主要负责管理材质

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IMaterialManager::IMaterialManager()
{
	m_pFatherScene = nullptr;
	m_pMaterialList = new std::vector<N_Material>;
	m_pDefaultMaterial = new N_Material;
	m_pMaterialHashTable = new std::unordered_map<std::string, UINT>;

	m_pDefaultMaterial->baseMaterial.mBaseAmbientColor = NVECTOR3(0, 0, 0);
	m_pDefaultMaterial->baseMaterial.mBaseDiffuseColor = NVECTOR3(0.1f, 0.1f, 0.1f);
	m_pDefaultMaterial->baseMaterial.mBaseSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
	m_pDefaultMaterial->baseMaterial.mEnvironmentMapTransparency = 0.0f;
	m_pDefaultMaterial->baseMaterial.mNormalMapBumpIntensity = 0.1f;
	m_pDefaultMaterial->baseMaterial.mSpecularSmoothLevel = 10;
	m_pDefaultMaterial->mMatName = std::string(NOISE_MACRO_DEFAULT_MATERIAL_NAME);
	m_pDefaultMaterial->diffuseMapName = "";
	m_pDefaultMaterial->specularMapName = "";
	m_pDefaultMaterial->environmentMapName = "";
	m_pDefaultMaterial->normalMapName = "";

	//well...all objects with invalid mat ID will use default material 
	m_pMaterialHashTable->insert(std::make_pair(NOISE_MACRO_DEFAULT_MATERIAL_NAME, NOISE_MACRO_INVALID_MATERIAL_ID));
}

UINT IMaterialManager::CreateMaterial(N_Material newMat)
{

	UINT newMatID = m_pMaterialList->size();

	if (newMat.mMatName == std::string(""))
	{
		//material name empty
		ERROR_MSG("Noise Material Mgr: material name empty!! At least 1 char !");
		return NOISE_MACRO_INVALID_MATERIAL_ID;
	}


	//GetMatID() will return DEFAULT_MAT_ID if material with new name has been found
	if(GetMatID(newMat.mMatName) != NOISE_MACRO_INVALID_MATERIAL_ID)
	{
		//material name conflict
			ERROR_MSG("Noise Material Mgr: material name existed!!It must be unique.");
			return NOISE_MACRO_INVALID_MATERIAL_ID;
	}

	//we can finally add a new material
	m_pMaterialList->push_back(newMat);

	//don't forget to add "name-index" map to hash table to accelerate element searching
	m_pMaterialHashTable->insert(std::make_pair(newMat.mMatName, newMatID));

	return newMatID;
}

UINT IMaterialManager::GetMatID(std::string  matName)
{
	auto stringIndexPairIter = m_pMaterialHashTable->find(matName);
	//if material not found
	if (stringIndexPairIter != m_pMaterialHashTable->end())
	{
		return stringIndexPairIter->second;//string-index pair, return UINT index for accessing vector
	}
	else
	{
		return NOISE_MACRO_INVALID_MATERIAL_ID;//invalid vertex Name
	}
}

void IMaterialManager::GetNameByIndex(UINT index, std::string&  outMatName)
{
	if (index < m_pMaterialList->size())
	{
		//return the name of corresponding material
		outMatName = m_pMaterialList->at(index).mMatName;
	}
}

void IMaterialManager::SetMaterial(UINT matIndex, N_Material newMat)
{
	if (matIndex < m_pMaterialList->size())
	{
		m_pMaterialList->at(matIndex) = newMat;
	}
};

void IMaterialManager::SetMaterial(std::string  matName, N_Material newMat)
{
	UINT matID = GetMatID(matName);
	if (matID != NOISE_MACRO_INVALID_MATERIAL_ID)
	{
		SetMaterial(matID, newMat);
	}

};

UINT IMaterialManager::GetMaterialCount()
{
	return m_pMaterialList->size();
}

void IMaterialManager::GetDefaultMaterial(N_Material & outMat)
{
	outMat = *m_pDefaultMaterial;
}

void IMaterialManager::GetMaterial(UINT matID,N_Material& outMat)
{
	UINT validatedMatID = mFunction_ValidateMaterialID(matID);
	if (validatedMatID != NOISE_MACRO_INVALID_MATERIAL_ID)
	{
		outMat = m_pMaterialList->at(matID);
	}
};


BOOL IMaterialManager::DeleteMaterial(UINT matID)
{
	UINT validatedID = mFunction_ValidateMaterialID(matID);
	if (validatedID != NOISE_MACRO_INVALID_MATERIAL_ID)
	{
		//not only the real data in std::vector should be erased, but also
		//the NAME-INDEX mapping in hash table
		std::string deleteMatName = m_pMaterialList->at(validatedID).mMatName;
		auto nameIndexMapIter = m_pMaterialHashTable->find(deleteMatName);
		//..............(the check might be useless...)
		if (nameIndexMapIter != m_pMaterialHashTable->end())
		{
			m_pMaterialHashTable->erase(nameIndexMapIter);
		}

		//delete real Material data in std::Vector
		auto tmpIter = m_pMaterialList->begin();
		tmpIter += validatedID;
		m_pMaterialList->erase(tmpIter);

		//update hash table (string-index pair, index should be updated)
		mFunction_RefreshHashTableAfterDeletion(validatedID, 1);
	}
	else
	{
		ERROR_MSG("Delete Material : material Index invalid!!");
		return FALSE;
	}

	return TRUE;
};

BOOL IMaterialManager::DeleteMaterial(std::string matName)
{
	auto nameIndexMapIter = m_pMaterialHashTable->find(matName);

	//if material name is found
	if (nameIndexMapIter != m_pMaterialHashTable->end())
	{
		//string-UINT pair
		UINT matIndex = nameIndexMapIter->second;

		//delete material data in vector
		UINT validatedID = mFunction_ValidateMaterialID(matIndex);
		if (validatedID != NOISE_MACRO_INVALID_MATERIAL_ID)
		{
			auto tmpIter = m_pMaterialList->begin() + validatedID;
			m_pMaterialList->erase(tmpIter);
		}

		//delete name-index pair
		m_pMaterialHashTable->erase(nameIndexMapIter);

		//update hash table (string-index pair, index should be updated)
		mFunction_RefreshHashTableAfterDeletion(validatedID,1);
	}
	else
	{
		//material not found
		ERROR_MSG("delete Material : material not found!!!");
		return FALSE;
	}

	return TRUE;
}

inline UINT IMaterialManager::ValidateIndex(UINT index)
{
	return mFunction_ValidateMaterialID(index);
};

/**********************************************************
							P R I V A T E
*************************************************************/

void IMaterialManager::Destroy()
{
	m_pFatherScene = nullptr;
}

inline UINT IMaterialManager::mFunction_ValidateMaterialID(UINT matID)
{
	if (matID < m_pMaterialList->size())
	{
		return matID;
	}
	else
	{
		return NOISE_MACRO_INVALID_MATERIAL_ID;
	}
}

inline void IMaterialManager::mFunction_RefreshHashTableAfterDeletion(UINT deletedMatID_threshold, UINT indexDecrement)
{
	for (auto& pair : *m_pMaterialHashTable)
	{
		if (pair.second>deletedMatID_threshold)pair.second -= indexDecrement;
	}
};


