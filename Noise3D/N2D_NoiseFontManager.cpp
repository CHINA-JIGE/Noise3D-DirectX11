/***********************************************************************

								class£ºNoiseFontLoader
		description : load /process /provide font texture(pixel matrix)
		

************************************************************************/
#include "Noise3D.h"

NoiseFontManager::NoiseFontManager()
{
	mIsFTInitialized = FALSE;
	m_FTLibrary		= nullptr;
	m_pFTFaceList = new std::vector<N_FontObject>;
	mFunction_InitFreeType();
}

void NoiseFontManager::SelfDestruction()
{

	for (auto face : *m_pFTFaceList)
	{
		FT_Done_Face(face.mFtFace);
	}

	FT_Done_FreeType(m_FTLibrary);
};

UINT NoiseFontManager::CreateFontFromFile(const char * filePath, const char * fontName, UINT fontSize)
{
	if (!mIsFTInitialized)
	{
		BOOL initSuccessful = mFunction_InitFreeType();
		//failed re-init
		if (!initSuccessful)
		{
			DEBUG_MSG1("FontMgr : Re-initialized failed!");
			return NOISE_MACRO_INVALID_ID;
		}
	}

	std::fstream tmpFile(filePath);
	if (!tmpFile.is_open())
	{
		DEBUG_MSG1("CreateFont : file path not exist..");
	}

	//font name must not be used
	for (auto fontObj : *m_pFTFaceList)
	{
		//string cmp
		if (fontObj.mFontName == std::string(fontName))
		{
			DEBUG_MSG1("CreateFont : Font Name has been used!");
			return NOISE_MACRO_INVALID_ID;
		}
	}

	//....
	N_FontObject	tmpFontObj;
	tmpFontObj.mFontName = fontName;


	//Create a FT_Face (to store font/char  info)
	FT_Error ftCreateNewFaceErr = FT_New_Face(
		m_FTLibrary,
		filePath,
		0,		//which style	,	regular/italic/bold/italic_bold
		&tmpFontObj.mFtFace);

	if (ftCreateNewFaceErr)
	{
		DEBUG_MSG1("FontLoader : Create Font failed!");
		return NOISE_MACRO_INVALID_ID;
	}
	else
	{
		mIsFTInitialized = TRUE;
	};
	
	//font size should be multiplied by 64 .... I don't know why but it's how it works
	FT_Set_Char_Size(tmpFontObj.mFtFace, UINT(fontSize/1.414) << 6,fontSize << 6, 64, 96);
	FT_Matrix    fontTransMatrix;              /* transformation matrix */

	FT_Vector     pen;

	//set a rotation matrix
	float angle = 0;
	fontTransMatrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	fontTransMatrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	fontTransMatrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	fontTransMatrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	FT_Set_Transform(tmpFontObj.mFtFace, &fontTransMatrix, &pen);


	//at last , store newly created face to a vector
	m_pFTFaceList->push_back(tmpFontObj);
	UINT newFontIndex = m_pFTFaceList->size() - 1;

	return newFontIndex;
};

void NoiseFontManager::GetBitmapOfChar(UINT fontID, wchar_t targetWChar, N_Font_Bitmap& outFontBitmap, NVECTOR4 backColor, NVECTOR4 textColor)
{
	if (!mFunction_ValidateFontID(fontID))
	{
		DEBUG_MSG1("FontMgr : GetPixelMatrix : FontID invalid!");
		return;
	}

	//Load Glyph from the target Unicode 
	FT_Error ftErr;
	//get corresponding ID of the wchar
	FT_UInt	 glyph_index = FT_Get_Char_Index(m_pFTFaceList->at(fontID).mFtFace, targetWChar);
	ftErr = FT_Load_Glyph(
		m_pFTFaceList->at(fontID).mFtFace,
		glyph_index,
		FT_LOAD_DEFAULT);

	//Get the Glyph of char
	FT_Glyph glyph;
	if (FT_Get_Glyph(m_pFTFaceList->at(fontID).mFtFace->glyph, &glyph))
		throw std::runtime_error("FT_Get_Glyph failed");
	
	//convert (and render) to a bitmap buffer
	FT_Render_Glyph(m_pFTFaceList->at(fontID).mFtFace->glyph, FT_RENDER_MODE_NORMAL);
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	//use the FT_Bitmap's 'buffer' to retrieve data
	FT_Bitmap& charBitmap = bitmap_glyph->bitmap;

	//assign color values to user-defined color buffer ref
	int charWidth = charBitmap.width;
	int charHeight = charBitmap.rows;

	//output to references
	for (int i = 0;i < charWidth*charHeight;i++)
	{
		if (charBitmap.buffer[i])
		{
			//add colors to user-defined buffer (greyScaleIntensity * color)
			outFontBitmap.bitmapBuffer.push_back(
				NVECTOR4(
					float(charBitmap.buffer[i]) / 256.0f *textColor.x,
					float(charBitmap.buffer[i]) / 256.0f* textColor.y,
					float(charBitmap.buffer[i]) / 256.0f* textColor.z,
					textColor.w));
		}
		else
		//current pixel  is black (not in glyph)
		{
			outFontBitmap.bitmapBuffer.push_back(backColor);
		}
	}
	outFontBitmap.height = charHeight;
	outFontBitmap.width = charWidth;
}

void NoiseFontManager::GetBitmapOfString(UINT fontID, std::string targetString, N_Font_Bitmap & outFontBitmap, NVECTOR4 backColor, NVECTOR4 textColor)
{
}




/************************************************************************
										P R I V A T E
************************************************************************/

BOOL NoiseFontManager::mFunction_InitFreeType()
{

	//Init FreeType Library
	FT_Error ftInitError = FT_Init_FreeType(&m_FTLibrary);

	if (ftInitError)
	{
		DEBUG_MSG1("FontLoader init failed!");
		m_FTLibrary = nullptr;
		return FALSE;
	}
	else
	{
		mIsFTInitialized = TRUE;
	};

	return TRUE;
}

BOOL NoiseFontManager::mFunction_ValidateFontID(UINT fontID)
{
	if (fontID >= 0 && fontID < m_pFTFaceList->size())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	//default return 
	return TRUE;
}
