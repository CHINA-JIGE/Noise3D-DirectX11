/***********************************************************************

					h£ºNoiseFontLoader

************************************************************************/

#pragma once

struct N_FontObject
{
	//HFONT 		mHFont;
	FT_Face		mFtFace;
	std::string		mFontName;
};


public class _declspec(dllexport) NoiseFontManager
{
public:

	NoiseFontManager();

	void		SelfDestruction();

	UINT		CreateFontFromFile(const char* filePath, const char* fontName, UINT fontSize);

	void		GetBitmapOfChar(UINT fontID,wchar_t targetWChar,N_Font_Bitmap& outFontBitmap,NVECTOR4 backColor,NVECTOR4 textColor);

	void		GetBitmapOfString(UINT fontID, std::string targetString, N_Font_Bitmap& outFontBitmap, NVECTOR4 backColor, NVECTOR4 textColor);

private:

	BOOL		mFunction_InitFreeType();

	BOOL		mFunction_ValidateFontID(UINT fontID);

private:
	FT_Library		m_FTLibrary;
	BOOL			mIsFTInitialized;
	std::vector<N_FontObject>*			m_pFTFaceList;
};
