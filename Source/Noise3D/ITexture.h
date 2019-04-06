/***********************************************************************

									h£ºITexture
					basic/common texture interface
					inherit by Texture2D, TextureCubeMap

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*declspec(dllexport)*/ ITexture
	{
	public:

		uint32_t GetFaceCount() { return mArraySize; };

		N_UID GetTextureName() { return mTextureUid; }

		uint32_t GetWidth() { return mWidth; }

		uint32_t GetHeight() { return mHeight; }

		bool	IsSysMemBufferValid()const { return mIsPixelBufferInMemValid; }

		uint32_t GetMipMapLevels() { return mMipMapLevels; }

	protected:

		friend class IRenderInfrastructure;

		ITexture(): mArraySize(1),mWidth(0),mHeight(0),mTextureUid(""), mIsPixelBufferInMemValid(false){}

		~ITexture() 
		{	
			//safe_release SRV  interface
			ReleaseCOM(m_pSRV);
			mPixelBuffer.clear();
		};

		//called by TextureManager to init internal vars
		virtual void NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitTexture(
			ID3D11ShaderResourceView* pSRV,
			const N_UID& uid,
			std::vector<Color4u>&& pixelBuff,
			bool isSysMemBuffValid)=0;

		uint32_t mArraySize;//common=1, cubeMap=6
		uint32_t mMipMapLevels;
		uint32_t mMipMapChainPixelCount;//if this is a cube map(texture array), then this pitch is essential
		uint32_t mWidth;//pixel width
		uint32_t mHeight;//pixel height
		N_UID	mTextureUid;
		std::vector<Color4u> mPixelBuffer;//a copy of pixel data in system memory
		bool	mIsPixelBufferInMemValid;
		ID3D11ShaderResourceView*	m_pSRV;//used by renderer,but d3d detail must be covered

	};
}