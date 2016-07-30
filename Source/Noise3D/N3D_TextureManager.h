/***********************************************************************

                           h£ºNoiseTextureManager

************************************************************************/

#pragma once

namespace Noise3D
{
	struct N_TextureObject
	{
		N_TextureObject() { ZeroMemory(this, sizeof(*this)); }
		std::string			mTexName;
		std::vector<NVECTOR4>	mPixelBuffer;
		BOOL	mIsPixelBufferInMemValid;
		ID3D11ShaderResourceView*	m_pSRV;
		NOISE_TEXTURE_TYPE mTextureType;
	};

	class _declspec(dllexport) ITextureManager 
	{
	public:
		friend class IScene;
		friend class IRenderer;
		friend class IFontManager;//Let it Create\Access bitmap table Texture

		//¹¹Ôìº¯Êý
		ITextureManager();

		//-------Set Pixel of texture system memory
		BOOL	SetPixel_SysMem(UINT texID, UINT x, UINT y, const  NVECTOR4& color);

		//-------
		NVECTOR4	GetPixel_SysMem(UINT texID, UINT x, UINT y);

		//--------upload the pixel matrix (in memory) to GPU
		BOOL	UpdateTextureDataToGraphicMemory(UINT texID);

		BOOL	UpdateTextureDataToGraphicMemory(std::string texName);

		//--------
		UINT		CreatePureColorTexture(std::string texName, UINT pixelWidth, UINT pixelHeight, NVECTOR4 color, BOOL keepCopyInMemory = FALSE);

		//--------
		UINT		CreateTextureFromFile(NFilePath filePath, std::string texName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight, BOOL keepCopyInMemory = FALSE);

		//--------
		UINT		CreateCubeMapFromFiles(NFilePath fileName[6], std::string cubeTextureName, NOISE_CUBEMAP_SIZE faceSize);

		//--------
		UINT		CreateCubeMapFromDDS(NFilePath dds_FileName, std::string cubeTextureName, NOISE_CUBEMAP_SIZE faceSize);

		//--------
		BOOL	ConvertTextureToGreyMap(UINT texID);

		BOOL	ConvertTextureToGreyMap(std::string texName);

		//--------
		BOOL	ConvertTextureToGreyMapEx(UINT texID, float factorR, float factorG, float factorB);

		BOOL	ConvertTextureToGreyMapEx(std::string texName, float factorR, float factorG, float factorB);

		//--------
		BOOL	ConvertHeightMapToNormalMap(UINT texID, float heightFieldScaleFactor = 10.0f);

		BOOL	ConvertHeightMapToNormalMap(std::string texName, float heightFieldScaleFactor = 10.0f);

		//--------
		BOOL	SaveTextureToFile(UINT texID, NFilePath filePath, NOISE_TEXTURE_SAVE_FORMAT picFormat);

		BOOL	SaveTextureToFile(std::string texName, NFilePath filePath, NOISE_TEXTURE_SAVE_FORMAT picFormat);

		//--------
		UINT		GetTextureID(std::string texName);

		//--------
		void		GetTextureName(UINT index, std::string& outTextureName);

		//--------
		UINT		GetTextureWidth(UINT texID);

		UINT		GetTextureWidth(std::string texName);

		//--------
		UINT		GetTextureHeight(UINT texID);

		UINT		GetTextureHeight(std::string texName);

		//--------
		UINT		GetTextureCount();

		//--------
		BOOL	DeleteTexture(UINT texID);

		BOOL	DeleteTexture(std::string texName);

		//--------return original index if valid, return INVALID_ID otherwise
		UINT		ValidateIndex(UINT texID);

		UINT		ValidateIndex(UINT texID, NOISE_TEXTURE_TYPE texType);

	private:

		void		Destroy();

		void		mFunction_RefreshHashTableAfterDeletion(UINT deletedTexID_threshold, UINT indexDecrement);

		UINT		mFunction_CreateTextureFromFile_DirectlyLoadToGpu(NFilePath filePath, std::string& textureName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight);

		UINT		mFunction_CreateTextureFromFile_KeepACopyInMemory(NFilePath filePath, std::string& textureName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight);

		UINT		mFunction_GetPixelIndexFromXY(UINT x, UINT y, UINT width);

	private:
		IScene*								m_pFatherScene;
		std::vector<N_TextureObject>*	m_pTextureObjectList;
		//using index to access resource is inevitable, but only
		//NAME STRING can uniquely identify a resource, optimization must be 
		//implemented.
		std::unordered_map<std::string, UINT>*	m_pTextureObjectHashTable;
	};
}