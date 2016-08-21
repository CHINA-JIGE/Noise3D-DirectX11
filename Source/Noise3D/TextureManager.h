/***********************************************************************

                           h£ºNoiseTextureManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class N_TextureObject
	{
	public:

		//std::string			mTexName;
		std::vector<NVECTOR4>	mPixelBuffer;
		BOOL	mIsPixelBufferInMemValid;
		ID3D11ShaderResourceView*	m_pSRV;
		NOISE_TEXTURE_TYPE mTextureType;

	private:
		friend class IFactory<N_TextureObject>;

		N_TextureObject() { ZeroMemory(this, sizeof(*this)); }

		~N_TextureObject()
		{
			//safe_release SRV  interface
			ReleaseCOM(m_pSRV);
			mPixelBuffer.clear();
		}
	};

	class /*_declspec(dllexport)*/ ITextureManager :
		public IFactory<N_TextureObject>
	{
	public:
		friend class IRenderer;
		friend class IFontManager;//Let it Create\Access bitmap table Texture


		//-------Set Pixel of texture system memory
		BOOL	SetPixel_SysMem(UINT texID, UINT x, UINT y, const  NVECTOR4& color);

		//-------
		NVECTOR4	GetPixel_SysMem(UINT texID, UINT x, UINT y);

		//--------upload the pixel matrix (in memory) to GPU
		BOOL	UpdateTextureDataToGraphicMemory(UINT texID);

		BOOL	UpdateTextureDataToGraphicMemory(N_UID texName);

		//--------
		UINT		CreatePureColorTexture(N_UID texName, UINT pixelWidth, UINT pixelHeight, NVECTOR4 color, BOOL keepCopyInMemory = FALSE);

		//--------
		UINT		CreateTextureFromFile(NFilePath filePath, N_UID texName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight, BOOL keepCopyInMemory = FALSE);

		//--------
		UINT		CreateCubeMapFromFiles(NFilePath fileName[6], N_UID cubeTextureName, NOISE_CUBEMAP_SIZE faceSize);

		//--------
		UINT		CreateCubeMapFromDDS(NFilePath dds_FileName, N_UID cubeTextureName, NOISE_CUBEMAP_SIZE faceSize);

		//--------
		BOOL	ConvertTextureToGreyMap(UINT texID);

		BOOL	ConvertTextureToGreyMap(N_UID texName);

		//--------
		BOOL	ConvertTextureToGreyMapEx(UINT texID, float factorR, float factorG, float factorB);

		BOOL	ConvertTextureToGreyMapEx(N_UID texName, float factorR, float factorG, float factorB);

		//--------
		BOOL	ConvertHeightMapToNormalMap(UINT texID, float heightFieldScaleFactor = 10.0f);

		BOOL	ConvertHeightMapToNormalMap(N_UID texName, float heightFieldScaleFactor = 10.0f);

		//--------
		BOOL	SaveTextureToFile(UINT texID, NFilePath filePath, NOISE_TEXTURE_SAVE_FORMAT picFormat);

		BOOL	SaveTextureToFile(N_UID texName, NFilePath filePath, NOISE_TEXTURE_SAVE_FORMAT picFormat);

		//--------
		UINT		GetTextureID(N_UID texName);

		//--------
		N_UID	GetTextureName(UINT index);

		//--------
		UINT		GetTextureWidth(UINT texID);

		UINT		GetTextureWidth(N_UID texName);

		//--------
		UINT		GetTextureHeight(UINT texID);

		UINT		GetTextureHeight(N_UID texName);

		//--------
		UINT		GetTextureCount();

		//--------
		BOOL	DeleteTexture(UINT texID);

		BOOL	DeleteTexture(N_UID texName);

		void		DeleteAllTexture();

		//--------return original index if valid, return INVALID_ID otherwise
		UINT		ValidateIndex(UINT texID);

		UINT		ValidateIndex(UINT texID, NOISE_TEXTURE_TYPE texType);

		BOOL		ValidateUID(N_UID texName);

		BOOL		ValidateUID(N_UID texName, NOISE_TEXTURE_TYPE texType);

	private:

		friend IFactory<ITextureManager>;

		//¹¹Ôìº¯Êý
		ITextureManager();

		~ITextureManager();

		UINT		mFunction_CreateTextureFromFile_DirectlyLoadToGpu(NFilePath filePath, std::string& texName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight);

		UINT		mFunction_CreateTextureFromFile_KeepACopyInMemory(NFilePath filePath, std::string& texName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight);

		UINT		mFunction_GetPixelIndexFromXY(UINT x, UINT y, UINT width);

	};
}