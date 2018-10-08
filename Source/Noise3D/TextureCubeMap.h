
/***********************************************************************

									h£ºTextureCubeMap
		(2018.10.8)decided not to inherit from common texture,
		only provide limited interfaces for cube map manipulation

************************************************************************/

#pragma once

namespace Noise3D
{


	class /*declspec(dllexport)*/ TextureCubeMap:
		public ITexture
	{
	public:

		enum N_TEXTURE_CPU_SAMPLE_MODE
		{
			POINT,
			BILINEAR
		};

		NColor4u		GetPixel(uint32_t faceID, uint32_t x, uint32_t y);

		NColor4u		GetPixel(NVECTOR3 dir, N_TEXTURE_CPU_SAMPLE_MODE mode);

	private:

		friend  class TextureManager;

		friend IFactory<TextureCubeMap>;

		TextureCubeMap();

		~TextureCubeMap();
	
		//called by TextureManager to init internal vars
		virtual void NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitTexture(
			ID3D11ShaderResourceView* pSRV,
			const N_UID& uid,
			std::vector<NColor4u>&& pixelBuff,
			bool isSysMemBuffValid) override;
	};

};