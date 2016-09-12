
/***********************************************************************

                           h£ºITexture

************************************************************************/

#pragma once


namespace Noise3D
{

	class /*declspec(dllexport)*/ ITexture
	{
	public:

		BOOL				IsSysMemPixelBufferValid();

		N_UID				GetTextureName();

		NOISE_TEXTURE_TYPE GetTextureType();

		BOOL				IsTextureType(NOISE_TEXTURE_TYPE type);

		UINT				GetWidth();

		UINT				GetHeight();

		void					SetPixel(UINT x, UINT y, const NVECTOR4& color);

		NVECTOR4		GetPixel(UINT x, UINT y);

		BOOL				SetPixelArray(const std::vector<NVECTOR4>& in_ColorArray);//faster than setPixel() for every pixel because less check will be done

		BOOL				SetPixelArray(std::vector<NVECTOR4>&& in_ColorArray);//faster than setPixel() for every pixel because less check will be done

		BOOL				GetPixelArray(std::vector<NVECTOR4>& outColorArray);

		BOOL				UpdateToVideoMemory();

		BOOL				ConvertTextureToGreyMap();

		BOOL				ConvertTextureToGreyMapEx(float factorR, float factorG, float factorB);

		BOOL				ConvertHeightMapToNormalMap(float heightFieldScaleFactor = 10.0f);

		BOOL				SaveTextureToFile(NFilePath filePath, NOISE_TEXTURE_SAVE_FORMAT picFormat);

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
			BOOL isSysMemBuffValid,
			NOISE_TEXTURE_TYPE type);

		UINT		mWidth;
		UINT		mHeight;
		N_UID*		m_pTextureUid;
		std::vector<NVECTOR4>	mPixelBuffer;
		BOOL	mIsPixelBufferInMemValid;
		ID3D11ShaderResourceView*	m_pSRV;//used by renderer,but d3d detail must be covered
		NOISE_TEXTURE_TYPE mTextureType;

	};

};