/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IMaterialManager
	{
	public:
		friend  IScene;
		friend  IRenderer;

		//¹¹Ôìº¯Êý
		IMaterialManager();

		//-------
		UINT		CreateMaterial(N_Material newMat);

		//-------
		UINT		GetMatID(std::string matName);

		//-------
		void		GetNameByIndex(UINT index, std::string& outMatName);

		//-------
		void		SetMaterial(UINT matIndex, N_Material newMat);

		void		SetMaterial(std::string matName, N_Material newMat);

		//-------
		UINT		GetMaterialCount();

		//-------
		void		GetDefaultMaterial(N_Material& outMat);

		//-------
		void		GetMaterial(UINT matID, N_Material& outMat);

		//-------
		BOOL	DeleteMaterial(UINT matID);

		BOOL	DeleteMaterial(std::string matName);

		//--------return original index if valid, return INVALID_ID otherwise
		UINT		ValidateIndex(UINT index);

	private:

		void		Destroy();

		UINT		mFunction_ValidateMaterialID(UINT matID);

		void		mFunction_RefreshHashTableAfterDeletion(UINT deletedMatID_threshold, UINT indexDecrement);

		IScene*								m_pFatherScene;
		N_Material*								m_pDefaultMaterial;
		std::vector<N_Material>*			m_pMaterialList;
		std::unordered_map<std::string, UINT>*	m_pMaterialHashTable;
	};
}