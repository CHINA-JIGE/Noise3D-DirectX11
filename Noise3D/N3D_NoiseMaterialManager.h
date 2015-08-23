/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseMaterialManager
{
public:
	friend class NoiseScene;
	friend class NoiseRenderer;
	friend class NoiseMesh;
	friend class NoiseMaterialManager;

	//¹¹Ôìº¯Êý
	NoiseMaterialManager();
	~NoiseMaterialManager();

	UINT		CreateMaterial(N_Material newMat);

	UINT		GetIndexByName(std::string* matName);

	void		GetNameByIndex(UINT index, std::string* outMatName);

	void		SetMaterial(UINT matIndex,N_Material newMat);

	void		SetMaterial(std::string* matName,N_Material newMat);

	UINT		GetMaterialCount();


private:
	NoiseScene*								m_pFatherScene;
	N_Material*								m_pDefaultMaterial;
	std::vector<N_Material>*			m_pMaterialList;
};