/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once

struct N_Material_Object
{
	N_Material_Object() { ZeroMemory(this, sizeof(*this)); }
	std::string		mMatName;
	N_Material_Basic	mMat;
};

public class _declspec(dllexport) NoiseMaterialManager
{
public:
	friend NoiseScene;
	friend NoiseRenderer;
	friend NoiseMesh;

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