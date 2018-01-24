/***********************************************************************

							h£ºNoise2DText

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IStaticText :
			public IBasicContainerInfo,
			public IBasicTextInfo
		{
		public:

			NVECTOR2 GetFontSize();

		private:

			friend class IRenderModuleForText;
			friend class IFontManager;
			friend IFactory<IStaticText>;

			IStaticText();

			~IStaticText();

			void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGraphicObject(IGraphicObject* pCreatedObj,UINT pxWidth, UINT pxHeight, NVECTOR4 color, N_UID texName);

			void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateGraphicObject();//extern call by renderer

			N_UID	mTextureName;//which bitmap texture to refer to

			N_UID	mFontName;
		};
}