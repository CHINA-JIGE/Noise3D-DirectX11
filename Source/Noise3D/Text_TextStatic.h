/***********************************************************************

							h£ºNoise2DText

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IStaticText :
			public CBasicContainerInfo,
			public IBasicTextInfo
		{
		public:

			friend class IRenderer;
			friend class IFontManager;

			IStaticText();

			NVECTOR2 GetFontSize(UINT fontID);


		private:

			void		Destroy();

			void		mFunction_InitGraphicObject(UINT pxWidth, UINT pxHeight, NVECTOR4 color, UINT texID);

			void		NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_UpdateGraphicObject();//extern call by renderer

		private:

			UINT					mStringTextureID;//to avoid performance overhead caused by GetTextureID;
			std::string*		m_pTextureName;//which bitmap texture to refer to
		};
}