/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once


class _declspec(dllexport) NoiseMaterialManager:public NoiseClassLifeCycle
{
public:
	friend class NoiseScene;
	friend class NoiseRenderer;
	friend class NoiseMesh;

	//¹¹Ôìº¯Êý
	NoiseMaterialManager();

	UINT		CreateMaterial(N_Material newMat);

	UINT		GetIndexByName(std::string* matName);

	void		GetNameByIndex(UINT index, std::string* outMatName);

	void		SetMaterial(UINT matIndex,N_Material newMat);

	void		SetMaterial(std::string* matName,N_Material newMat);

	UINT		GetMaterialCount();


private:

	void		Destroy();

	NoiseScene*								m_pFatherScene;
	N_Material*								m_pDefaultMaterial;
	std::vector<N_Material>*			m_pMaterialList;
};