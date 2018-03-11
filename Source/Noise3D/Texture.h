
/***********************************************************************

                           h£ºITexture

************************************************************************/

#pragma once

namespace Noise3D
{

	enum NOISE_TEXTURE_TYPE
	{
		NOISE_TEXTURE_TYPE_COMMON = 0,
		NOISE_TEXTURE_TYPE_CUBEMAP = 1,
		NOISE_TEXTURE_TYPE_VOLUME = 2,
	};

	enum NOISE_IMAGE_FILE_FORMAT
	{
		NOISE_IMAGE_FILE_FORMAT_BMP,//Supported by DirectXTex.WIC
		NOISE_IMAGE_FILE_FORMAT_JPG,//Supported by  DirectXTex.WIC
		NOISE_IMAGE_FILE_FORMAT_PNG,//Supported by  DirectXTex.WIC
		NOISE_IMAGE_FILE_FORMAT_TIFF,//Supported by  DirectXTex.WIC
		NOISE_IMAGE_FILE_FORMAT_GIF,//Supported by  DirectXTex.WIC
		NOISE_IMAGE_FILE_FORMAT_TGA,//supported by DirectXTex
		NOISE_IMAGE_FILE_FORMAT_HDR,//supported by DirectXTex
		NOISE_IMAGE_FILE_FORMAT_DDS,//supported by DirectXTex
		NOISE_IMAGE_FILE_FORMAT_NOT_SUPPORTED = 0xffff
	};

	class /*declspec(dllexport)*/ ITexture
	{
	public:

		bool				IsSysMemPixelBufferValid();

		N_UID				GetTextureName();

		NOISE_TEXTURE_TYPE GetTextureType();

		bool				IsTextureType(NOISE_TEXTURE_TYPE type);

		UINT			GetWidth();

		UINT			GetHeight();

		void				SetPixel(UINT x, UINT y, const NColor4u& color);

		NColor4u		GetPixel(UINT x, UINT y);

		bool				SetPixelArray(const std::vector<NColor4u>& in_ColorArray);//faster than setPixel() for every pixel because less check will be done

		bool				SetPixelArray(std::vector<NColor4u>&& in_ColorArray);//faster than setPixel() for every pixel because less check will be done

		bool				GetPixelArray(std::vector<NColor4u>& outColorArray);

		bool				UpdateToVideoMemory();//update image's memory data to video memory

		bool				ConvertTextureToGreyMap();

		bool				ConvertTextureToGreyMap(float factorR, float factorG, float factorB);

		bool				ConvertHeightMapToNormalMap(float heightFieldScaleFactor = 10.0f);

		bool				SaveTexture2DToFile(NFilePath filePath, NOISE_IMAGE_FILE_FORMAT picFormat);

	private:

		friend class IRenderInfrastructure;

		friend  class ITextureManager;

		friend IFactory<ITexture>;

		ITexture();

		~ITexture();
		
		void	  NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitTexture(
			ID3D11ShaderResourceView* pSRV,
			const N_UID& uid,
			std::vector<NColor4u>&& pixelBuff,
			bool isSysMemBuffValid,
			NOISE_TEXTURE_TYPE type);

		UINT		mWidth;
		UINT		mHeight;
		N_UID		mTextureUid;
		std::vector<NColor4u>	mPixelBuffer;//a copy of pixel data in system memory
		bool	mIsPixelBufferInMemValid;
		ID3D11ShaderResourceView*	m_pSRV;//used by renderer,but d3d detail must be covered
		NOISE_TEXTURE_TYPE mTextureType;

	};

};