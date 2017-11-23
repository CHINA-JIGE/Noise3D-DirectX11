
/***********************************************************************

                           h£ºNoiseTextureManager

************************************************************************/
#pragma once

namespace Noise3D
{
	enum NOISE_CUBEMAP_SIZE
	{
		NOISE_CUBEMAP_SIZE_64x64 = 0,
		NOISE_CUBEMAP_SIZE_128x128 = 1,
		NOISE_CUBEMAP_SIZE_256x256 = 2,
		NOISE_CUBEMAP_SIZE_512x512 = 3,
		NOISE_CUBEMAP_SIZE_1024x1024 = 4,
	};

	enum NOISE_TEXTURE_TYPE
	{
		NOISE_TEXTURE_TYPE_COMMON = 0,
		NOISE_TEXTURE_TYPE_CUBEMAP = 1,
		NOISE_TEXTURE_TYPE_VOLUMN = 2,
	};

	enum NOISE_TEXTURE_SAVE_FORMAT
	{
		NOISE_TEXTURE_SAVE_FORMAT_BMP = D3DX11_IFF_BMP,
		NOISE_TEXTURE_SAVE_FORMAT_JPG = D3DX11_IFF_JPG,
		NOISE_TEXTURE_SAVE_FORMAT_PNG = D3DX11_IFF_PNG,
		NOISE_TEXTURE_SAVE_FORMAT_DDS = D3DX11_IFF_DDS,
		NOISE_TEXTURE_SAVE_FORMAT_TIFF = D3DX11_IFF_TIFF,
		NOISE_TEXTURE_SAVE_FORMAT_GIF = D3DX11_IFF_GIF,
		NOISE_TEXTURE_SAVE_FORMAT_WMP = D3DX11_IFF_WMP,
	};

	class /*_declspec(dllexport)*/ ITextureManager :
		public IFactory<ITexture>
	{
	public:
		friend class IRenderer;
		//friend class IFontManager;//Let it Create\Access bitmap table Texture

		ITexture*		CreatePureColorTexture(N_UID texName, UINT pixelWidth, UINT pixelHeight, NVECTOR4 color, bool keepCopyInMemory = false);

		ITexture*		CreateTextureFromFile(NFilePath filePath, N_UID texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight, bool keepCopyInMemory = false);

		ITexture*		CreateCubeMapFromFiles(NFilePath fileName[6], N_UID cubeTextureName, NOISE_CUBEMAP_SIZE faceSize);

		ITexture*		CreateCubeMapFromDDS(NFilePath dds_FileName, N_UID cubeTextureName, NOISE_CUBEMAP_SIZE faceSize);

		ITexture*		GetTexture(N_UID texName);

		UINT			GetTextureCount();

		bool			DeleteTexture(ITexture* pTex);

		bool			DeleteTexture(N_UID texName);

		void				DeleteAllTexture();

		bool			ValidateUID(N_UID texName);

		bool			ValidateUID(N_UID texName, NOISE_TEXTURE_TYPE texType);

	private:

		friend IFactory<ITextureManager>;

		//¹¹Ôìº¯Êý
		ITextureManager();

		~ITextureManager();

		ITexture*		mFunction_CreateTextureFromFile_DirectlyLoadToGpu(NFilePath filePath, std::string& texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight);

		ITexture*		mFunction_CreateTextureFromFile_KeepACopyInMemory(NFilePath filePath, std::string& texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight);
	};
}