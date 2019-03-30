/***********************************************************************

								h£ºNoise2DText

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ DynamicText :
			public  IBasicContainerInfo,
			public	IBasicTextInfo
		{
		public:


			void		SetWidth(float w);

			void		SetHeight(float h);

			void		SetFont(N_UID fontName);

			N_UID		GetFontName();

			void		SetTextAscii(const std::string& newText);

			void		GetTextAscii(std::string& outString);

			void		SetLineSpacingOffset(int offset);

			int		GetLineSpacingOffset();

			void		SetSpacePixelWidth(int width);

			void		SetWordSpacingOffset(int offset);

			int		GetWordSpacingOffset();

			Vec2 GetWordLocalPosOffset(UINT wordIndex);//index of word in string,not ascii code

			Vec2 GetWordRealSize(UINT wordIndex);

			Vec2 GetFontSize(UINT fontID);

		private:

			friend class IRenderModuleForText;
			friend class TextManager;
			friend IFactory<DynamicText>;

			DynamicText();

			~DynamicText();

			void	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGraphicObject(GraphicObject* pCreatedObj, UINT pxWidth, UINT pxHeight, Vec4 color, N_UID texName);

			void  NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_UpdateGraphicObject();//by renderer

		private:

			N_UID			mFontName;
			UINT			mCharBoundarySizeX;//updated when SetFontID
			UINT			mCharBoundarySizeY;
			int				mWordSpacingOffset;
			int				mLineSpacingOffset;
			int				mSpacePixelWidth;//pixel width of char space ' '
			N_UID			mTextureName;//which bitmap texture to refer to
			std::string		mTextContent;//the target "string"
			bool				mIsTextContentChanged;
			bool				mIsSizeChanged;
		};
}