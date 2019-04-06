
/***********************************************************************

                           h£º Texture manager

************************************************************************/
#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ TextureManager :
		public IFactoryEx<Texture2D, TextureCubeMap>
	{
	public:
		Texture2D*	CreatePureColorTexture(N_UID texName, UINT pixelWidth, UINT pixelHeight, Vec4 color, bool keepCopyInMemory = false);

		Texture2D*	CreatePureColorTexture(N_UID texName, UINT pixelWidth, UINT pixelHeight, Color4u color, bool keepCopyInMemory = false);

		Texture2D*	CreateTextureFromFile(NFilePath filePath, N_UID texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight, bool keepCopyInMemory = false);

		TextureCubeMap*	CreateCubeMapFromDDS(NFilePath dds_FileName, N_UID cubeTextureName, bool keepCopyInMemory = false);

		Texture2D*	GetTexture2D(N_UID texName);

		TextureCubeMap*	GetTextureCubeMap(N_UID texName);

		uint32_t		GetTexture2DCount();

		uint32_t		GetTextureCubeMapCount();

		bool				DeleteTexture(Texture2D* pTex);

		bool				DeleteTexture(TextureCubeMap* pTex);

		bool				DeleteTexture2D(N_UID texName);

		bool				DeleteTextureCubeMap(N_UID texName);

		void				DeleteAllTexture();

		bool				ValidateTex2D(N_UID texName);

		bool				ValidateTexCubeMap(N_UID texName);

	private:

		friend class IRenderInfrastructure;

		friend IFactory<TextureManager>;

		TextureManager();

		~TextureManager();

		const DXGI_FORMAT c_DefaultPixelDxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		NOISE_IMAGE_FILE_FORMAT mFunction_GetImageFileFormat(const std::string& fileSubfix);

		bool mFunction_IsBlockCompressionFormat(DXGI_FORMAT f);
	};

}
