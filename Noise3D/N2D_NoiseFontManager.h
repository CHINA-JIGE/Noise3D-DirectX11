/***********************************************************************

					h£ºNoiseFontLoader

************************************************************************/

#pragma once

struct N_FontObject
{
	//HFONT 		mHFont;
	FT_Face		mFtFace;
	UINT				mFontSize;
	float				mAspectRatio;
	std::string		mFontName;
	std::vector	<NVECTOR2>	mAsciiCharSizeList;//elements will be added in GetBitmapOfChar
	//UINT				mAsciiBitmapTableTextureID; should be updated by TexMgr:GetTextureID
};


public class _declspec(dllexport) NoiseFontManager
{
public:
	friend	class NoiseScene;
	friend	class NoiseRenderer;
	friend	class Noise2DTextDynamic;
	friend	class Noise2DTextStatic;

	NoiseFontManager();

	void		SelfDestruction();

	BOOL	AddChildObjectToRenderList();//Renderer need to be valid

	UINT		CreateFontFromFile(const char* filePath, const char* fontName, UINT fontSize,float fontAspectRatio=0.707f);

	BOOL	SetFontSize(UINT fontID,UINT  fontSize);
	
	UINT		CreateStaticTextA(UINT fontID, std::string targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset, Noise2DTextStatic& pText);

	UINT		CreateStaticTextW(UINT fontID, std::wstring targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset,Noise2DTextStatic& pText );

	UINT		CreateDynamicTextA(UINT fontID, std::string targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset, Noise2DTextDynamic& pText);

	UINT		GetFontID(std::string fontName);

private:
	//init freetype library
	BOOL	mFunction_InitFreeType();
	//didn't validate fontID
	void		mFunction_GetBitmapOfChar(N_FontObject& fontObj, wchar_t targetWChar, N_Font_Bitmap& outFontBitmap, NVECTOR4 textColor);
	//used for creating Bitmap Table
	void		mFunction_GetBitmapOfString(N_FontObject& fontObj, std::wstring targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, N_Font_Bitmap & outFontBitmap, int wordSpacingOffset, int lineSpacingOffset);
	//......
	UINT		mFunction_ValidateFontID(UINT fontID);
	//Use GetBitmapOfString() to create ascii bitmap table
	UINT		mFunction_CreateTexture_AsciiBitmapTable(N_FontObject& fontObj, UINT charWidth, UINT charHeight);

private:

	NoiseScene*						m_pFatherScene;
	NoiseTextureManager*		m_pTexMgr;//internal texture manager
	FT_Library			m_FTLibrary;
	BOOL				mIsFTInitialized;
	std::vector<N_FontObject>*					m_pFontObjectList;
	std::vector<Noise2DTextDynamic*>*		m_pChildTextDynamic;
	std::vector<Noise2DTextStatic*>*			m_pChildTextStatic;
};
