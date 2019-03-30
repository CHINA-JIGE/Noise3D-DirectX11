/***********************************************************************

							h£ºNoise2DText

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ StaticText :
			public IBasicContainerInfo,
			public IBasicTextInfo
		{
		public:

			Vec2 GetFontSize();

		private:

			friend class IRenderModuleForText;
			friend class TextManager;
			friend IFactory<StaticText>;

			StaticText();

			~StaticText();

			void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGraphicObject(GraphicObject* pCreatedObj,UINT pxWidth, UINT pxHeight, Vec4 color, N_UID texName);

			void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateGraphicObject();//extern call by renderer

			N_UID	mTextureName;//which bitmap texture to refer to

			N_UID	mFontName;
		};
}