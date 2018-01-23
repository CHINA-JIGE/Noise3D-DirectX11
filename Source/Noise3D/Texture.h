
/***********************************************************************

                           h£ºITexture

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

	class /*declspec(dllexport)*/ ITexture
	{
	public:

		bool				IsSysMemPixelBufferValid();

		N_UID				GetTextureName();

		NOISE_TEXTURE_TYPE GetTextureType();

		bool				IsTextureType(NOISE_TEXTURE_TYPE type);

		UINT				GetWidth();

		UINT				GetHeight();

		void					SetPixel(UINT x, UINT y, const NVECTOR4& color);

		NVECTOR4		GetPixel(UINT x, UINT y);

		bool				SetPixelArray(const std::vector<NVECTOR4>& in_ColorArray);//faster than setPixel() for every pixel because less check will be done

		bool				SetPixelArray(std::vector<NVECTOR4>&& in_ColorArray);//faster than setPixel() for every pixel because less check will be done

		bool				GetPixelArray(std::vector<NVECTOR4>& outColorArray);

		bool				UpdateToVideoMemory();

		bool				ConvertTextureToGreyMap();

		bool				ConvertTextureToGreyMapEx(float factorR, float factorG, float factorB);

		bool				ConvertHeightMapToNormalMap(float heightFieldScaleFactor = 10.0f);

		bool				SaveTextureToFile(NFilePath filePath, NOISE_TEXTURE_SAVE_FORMAT picFormat);

	private:

		friend class IRenderer;

		friend  class ITextureManager;

		friend IFactory<ITexture>;

		ITexture();

		~ITexture();
		
		void	  NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitTexture(
			ID3D11ShaderResourceView* pSRV,
			const N_UID& uid,
			std::vector<NVECTOR4>&& pixelBuff,
			bool isSysMemBuffValid,
			NOISE_TEXTURE_TYPE type);

		UINT		mWidth;
		UINT		mHeight;
		N_UID*		m_pTextureUid;
		std::vector<NVECTOR4>	mPixelBuffer;
		bool	mIsPixelBufferInMemValid;
		ID3D11ShaderResourceView*	m_pSRV;//used by renderer,but d3d detail must be covered
		NOISE_TEXTURE_TYPE mTextureType;

	};

};