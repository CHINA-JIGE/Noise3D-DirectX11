/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{

	struct N_Material_Basic
	{
		N_Material_Basic()
		{
			ZeroMemory(this, sizeof(*this));
			mBaseAmbientColor = NVECTOR3(0, 0, 0);
			mBaseDiffuseColor = NVECTOR3(0.3f, 1.0f, 0.3f);
			mBaseSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
			mSpecularSmoothLevel = 10;
			mNormalMapBumpIntensity = 0.1f;
			mEnvironmentMapTransparency = 0.3f;
		}

		//base attribute offset that will be added to final color
		NVECTOR3	mBaseAmbientColor;	INT32	mSpecularSmoothLevel;
		NVECTOR3	mBaseDiffuseColor;		float		mNormalMapBumpIntensity;
		NVECTOR3	mBaseSpecularColor;	float		mEnvironmentMapTransparency;

	};

	struct N_Material
	{
		N_Material() :
			mMatName(""),
			diffuseMapName(""),
			normalMapName(""),
			specularMapName(""),
			environmentMapName("")
		{ };

		N_UID	 mMatName;
		N_Material_Basic baseMaterial;
		N_UID diffuseMapName;
		N_UID normalMapName;
		N_UID specularMapName;
		N_UID environmentMapName;
	};


	class /*_declspec(dllexport)*/ IMaterialManager
	{
	public:
		friend  class IScene;
		friend  class IRenderer;

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