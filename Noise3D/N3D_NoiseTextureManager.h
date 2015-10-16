/***********************************************************************

                           h£ºNoiseTextureManager

************************************************************************/

#pragma once

struct N_Texture_Object
{
	N_Texture_Object() { ZeroMemory(this,sizeof(*this)); }
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

	UINT		CreateCubeMapFromFiles(LPCWSTR fileName[6], char* cubeTextureName,NOISE_CUBEMAP_SIZE faceSize);

	UINT		CreateCubeMapFromDDS(LPCWSTR dds_FileName, char * cubeTextureName, NOISE_CUBEMAP_SIZE faceSize);

	UINT		GetIndexByName(char* textureName);

	void		GetNameByIndex(UINT index, std::string* outTextureName);

	UINT		GetTextureCount();

	BOOL	DeleteTexture(UINT texID);

private:
	UINT		mFunction_ValidateTextureID(UINT texID, NOISE_TEXTURE_TYPE texType);

	UINT		mFunction_CreateTextureFromFile(LPCWSTR filePath, char* textureName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight, D3D11_USAGE bufferUsage, UINT cpuAccessFlag);


private:
	NoiseScene*									m_pFatherScene;
	std::vector<N_Texture_Object>*	m_pTextureObjectList;

};