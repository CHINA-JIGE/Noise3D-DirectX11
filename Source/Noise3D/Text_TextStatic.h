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

			NVECTOR2 GetFontSize();


		private:

			void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGraphicObject(IGraphicObject* pCreatedObj,UINT pxWidth, UINT pxHeight, NVECTOR4 color, N_UID texName);

			void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateGraphicObject();//extern call by renderer

		private:

			N_UID*				m_pTextureName;//which bitmap texture to refer to
			N_UID*				m_pFontName;
		};
}