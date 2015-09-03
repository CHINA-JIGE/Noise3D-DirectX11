/***********************************************************************

                           h£ºNoiseTextureManager

************************************************************************/

#pragma once

struct N_Texture_Object
{
	N_Texture_Object() { ZeroMemory(this,sizeof(*this)); }
	~N_Texture_Object() { ReleaseCOM(m_pSRV); }
	std::string	 mTexName;
	ID3D11ShaderResourceView* m_pSRV;
};

public class _declspec(dllexport) NoiseTextureManager
{
public:
	friend NoiseScene;
	friend NoiseRenderer;
	friend NoiseMesh;

	//¹¹Ôìº¯Êý
	NoiseTextureManager();

	void		SelfDestruction();

	UINT		CreateTextureFromFile(LPCWSTR filePath,char* textureName,BOOL useDefaultSize, UINT pixelWidth,UINT pixelHeight);

	UINT		GetIndexByName(char* textureName);

	void		GetNameByIndex(UINT index, std::string* outTextureName);

	UINT		GetTextureCount();

private:
	NoiseScene*									m_pFatherScene;
	std::vector<N_Texture_Object>*	m_pTextureObjectList;
};