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

		bool	IsSysMemBufferValid() { return mIsPixelBufferInMemValid; }

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
			std::vector<NColor4u>&& pixelBuff,
			bool isSysMemBuffValid)=0;

		UINT mArraySize;//common=1, cubeMap=6
		UINT mWidth;//pixel width
		UINT mHeight;//pixel height
		N_UID	mTextureUid;
		std::vector<NColor4u> mPixelBuffer;//a copy of pixel data in system memory
		bool	mIsPixelBufferInMemValid;
		ID3D11ShaderResourceView*	m_pSRV;//used by renderer,but d3d detail must be covered

	};
}