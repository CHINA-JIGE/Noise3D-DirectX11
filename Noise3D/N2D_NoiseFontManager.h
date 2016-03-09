/***********************************************************************

					h£ºNoiseFontLoader

************************************************************************/

#pragma once

struct N_Font_Bitmap
{
	UINT width;
	UINT height;
	std::vector<NVECTOR4> bitmapBuffer;
};

struct N_FontObject
{
	FT_Face		mFtFace;
	UINT				mFontSize;
	float				mAspectRatio;
	std::string		mFontName;//used by GetFontID;
	std::string		mInternalTextureName;//name in texture mgr
	std::vector	<NVECTOR2>	mAsciiCharSizeList;//elements will be added in GetBitmapOfChar

	//UINT				mAsciiBitmapTableTextureID; should be updated by TexMgr:GetTextureID
};


class _declspec(dllexport) NoiseFontManager:public NoiseClassLifeCycle
{
public:
	friend   class NoiseRenderer;
	friend	class Noise2DTextDynamic;
	friend	class Noise2DTextStatic;

	NoiseFontManager();

	BOOL	Initialize();

	UINT		CreateFontFromFile(NFilePath filePath, const char* fontName, UINT fontSize,float fontAspectRatio=0.707f);

	BOOL	SetFontSize(UINT fontID,UINT  fontSize);
	
	UINT		InitStaticTextA(UINT fontID, std::string targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset, Noise2DTextStatic& pText);

	UINT		InitStaticTextW(UINT fontID, std::wstring targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset,Noise2DTextStatic& pText );

	UINT		InitDynamicTextA(UINT fontID, std::string targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset, Noise2DTextDynamic& pText);

	UINT		GetFontID(std::string fontName);

	NVECTOR2 GetFontSize(UINT fontID);

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

	void		Destroy();

	NoiseTextureManager*		m_pTexMgr;//internal texture manager (ascii bitmap table/static Bitmap)
	FT_Library			m_FTLibrary;
	BOOL				mIsFTInitialized;
	std::vector<N_FontObject>*					m_pFontObjectList;
	std::vector<Noise2DTextDynamic*>*		m_pChildTextDynamic;
	std::vector<Noise2DTextStatic*>*			m_pChildTextStatic;
};
