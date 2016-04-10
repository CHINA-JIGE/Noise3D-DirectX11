/***********************************************************************

								class£ºNoiseFontLoader
		description : load /process /provide font texture(pixel matrix)
		

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IFontManager::IFontManager()
{
	mIsFTInitialized = FALSE;
	m_FTLibrary		= nullptr;
	m_pTexMgr = new ITextureManager;
	m_pFontObjectList = new std::vector<N_FontObject>;
	m_pChildTextDynamic = new std::vector<IDynamicText*>;
	m_pChildTextStatic = new std::vector<IStaticText*>;
}

BOOL IFontManager::Initialize()
{
	BOOL isFTInitSucceeded=mFunction_InitFreeType();
	if (!isFTInitSucceeded)
	{
		DEBUG_MSG1("Font Manager Initialize failed!!");
		return FALSE;
	}

	SetStatusToBeInitialized();
	return TRUE;
}

void IFontManager::Destroy()
{

		for (auto face : *m_pFontObjectList)
		{
			FT_Done_Face(face.mFtFace);
		}

		FT_Done_FreeType(m_FTLibrary);
		m_pTexMgr->SelfDestruction();

		for (auto text : *m_pChildTextDynamic)
		{
			text->SelfDestruction();
		}
		for (auto text : *m_pChildTextStatic)
		{
			text->SelfDestruction();
		}
};


UINT	 IFontManager::CreateFontFromFile(NFilePath filePath, const char * fontName, UINT fontSize, float fontAspectRatio)
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
	for (auto fontObj : *m_pFontObjectList)
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
		tmpFontObj.mFontSize = fontSize;
		tmpFontObj.mAspectRatio = fontAspectRatio;

	//Create a FT_Face (to store font/char  info)
	FT_Error ftCreateNewFaceErr = FT_New_Face(
		m_FTLibrary,
		filePath.c_str(),
		0,		//which style	,	regular/italic/bold/italic_bold (maybe available)
		&tmpFontObj.mFtFace);

		if (ftCreateNewFaceErr)
	{
		DEBUG_MSG1("FontMgr : Create Font failed!");
		return NOISE_MACRO_INVALID_ID;
	}
	else
	{
		mIsFTInitialized = TRUE;
	};

	//font size should be multiplied by 64 .... I don't know why but it's how it works
	FT_Set_Pixel_Sizes(tmpFontObj.mFtFace, UINT(fontSize / 1.414), fontSize);
	//FT_Set_Char_Size(tmpFontObj.mFtFace, UINT(fontSize / 1.414) << 6, fontSize << 6, 72, 72);


	FT_Matrix    fontTransMatrix;              /* transformation matrix */
	FT_Vector     pen;

	//set a rotation matrix
	float angle = 0.0f;
	fontTransMatrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	fontTransMatrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	fontTransMatrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	fontTransMatrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	FT_Set_Transform(tmpFontObj.mFtFace, &fontTransMatrix, &pen);


	//------------new Font Object--------------
	//Create Bitmap Table
	UINT bitmapTableTexID = mFunction_CreateTexture_AsciiBitmapTable(
		tmpFontObj,
		UINT(fontSize*fontAspectRatio),
		fontSize);//bitmap size (height) for 1 ascii char

	if (bitmapTableTexID == NOISE_MACRO_INVALID_ID)
	{
		DEBUG_MSG1("CreateFont : create bitmap table failed!!");
		return NOISE_MACRO_INVALID_ID;
	}

	m_pFontObjectList->push_back(tmpFontObj);
	UINT newFontIndex = m_pFontObjectList->size() - 1;

	return newFontIndex;
}

BOOL IFontManager::SetFontSize(UINT fontID, UINT  fontSize)
{
	if (mFunction_ValidateFontID(fontID) != NOISE_MACRO_INVALID_ID)
	{
		if (fontSize < 4)fontSize = 4;
		m_pFontObjectList->at(fontID).mFontSize = fontSize;
		//font size should be multiplied by 64 .... I don't know why but it's how it works
		//FT_Set_Pixel_Sizes(tmpFontObj.mFtFace, UINT(fontSize / 1.414), fontSize);
		FT_Set_Char_Size(m_pFontObjectList->at(fontID).mFtFace, UINT(fontSize / 1.414) << 6, fontSize << 6, 72, 72);


		return TRUE;
	}
	return FALSE;
}

UINT	 IFontManager::InitStaticTextA(UINT fontID, std::string targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset, IStaticText & refText)
{
	UINT returnedTextID = NOISE_MACRO_INVALID_ID;
	std::wstring tmpWString;
	tmpWString.assign(targetString.begin(), targetString.end());
	returnedTextID = InitStaticTextW(fontID, tmpWString, boundaryWidth, boundaryHeight, textColor, wordSpacingOffset, lineSpacingOffset, refText);
	return returnedTextID;
};

UINT	 IFontManager::InitStaticTextW(UINT fontID, std::wstring targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset, IStaticText& refText)
{
	//dynamic text use bitmap table & texture coordinate to  render text
	if (refText.IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Initialized!");
		return NOISE_MACRO_INVALID_ID;
	}



	//validate font ID
	UINT validatedFontID = mFunction_ValidateFontID(fontID);
	if (validatedFontID == NOISE_MACRO_INVALID_ID)
	{
		refText.Destroy();
		DEBUG_MSG1("InitStaticTextW:Font ID Invalid!!");
		return NOISE_MACRO_INVALID_ID;
	}

	//texture name in	TexMgr
	std::stringstream tmpTextureName;
	tmpTextureName << "Internal_StaticTextTexture" << (m_pTexMgr->m_pTextureObjectList->size()-1);//texture name generated with font ID

	//Create a pure color Texture
	UINT stringTextureID = NOISE_MACRO_INVALID_TEXTURE_ID;
	stringTextureID = m_pTexMgr->CreatePureColorTexture(
		tmpTextureName.str().c_str(),
		boundaryWidth,
		boundaryHeight,
		NVECTOR4(0, 0, 0, 0),
		TRUE
		);

	//check if texture creation success
	if (stringTextureID == NOISE_MACRO_INVALID_TEXTURE_ID)
	{
		refText.Destroy();
		DEBUG_MSG1("InitStaticTextW : Create Bitmap Table Texture failed!");
		return NOISE_MACRO_INVALID_ID;
	}

	//now get the whole bitmap from user-input string
	N_Font_Bitmap tmpFontBitmap;
	mFunction_GetBitmapOfString(
		m_pFontObjectList->at(fontID),//font ID validated
		targetString,
		boundaryWidth,
		boundaryHeight,
		textColor,
		tmpFontBitmap,
		0,
		0);

	//copy bitmap to texture 
	m_pTexMgr->m_pTextureObjectList->at(stringTextureID).mPixelBuffer.assign(
		tmpFontBitmap.bitmapBuffer.begin(),
		tmpFontBitmap.bitmapBuffer.end()
		);

	//update a texture in the identity of FONT MGR (which match the Required Access Permission)
	BOOL UpdateToGMSuccess = FALSE;
	UpdateToGMSuccess = m_pTexMgr->UpdateTextureDataToGraphicMemory(stringTextureID);
	if (!UpdateToGMSuccess)
	{
		refText.Destroy();
		DEBUG_MSG1("InitStaticTextW : Create Text Bitmap failed!!");
		m_pTexMgr->DeleteTexture(stringTextureID);
		return NOISE_MACRO_INVALID_ID;
	}


	//------------initialize the graphic object of the TEXT------------
	//m_pFatherScene->CreateGraphicObject(*refText.m_pGraphicObj);
	refText.mFunction_InitGraphicObject(boundaryWidth, boundaryHeight, textColor, stringTextureID);


	//....only after all init work was done can we bind mgr/object together

	*(refText.m_pTextureName)=tmpTextureName.str();
	m_pChildTextStatic->push_back(&refText);
	refText.m_pFatherFontMgr = this;
	refText.SetStatusToBeInitialized();

	return m_pChildTextStatic->size() - 1;
}

UINT	 IFontManager::InitDynamicTextA(UINT fontID, std::string targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset,IDynamicText& refText)
{
	//dynamic text use bitmap table & texture coordinate to  render text
	if (refText.IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Initialized!");
		return NOISE_MACRO_INVALID_ID;
	}


	//validate font ID
	UINT validatedFontID = mFunction_ValidateFontID(fontID);
	if (validatedFontID == NOISE_MACRO_INVALID_ID)
	{
		refText.Destroy();
		DEBUG_MSG1("CreateDynamicText:Font ID Invalid!!");
		return NOISE_MACRO_INVALID_ID;
	}

	//Get texID of bitmap table (ID may change, but not name)
	//UINT stringTextureID = m_pTexMgr->GetTextureID(m_pFontObjectList->at(fontID).mFontName);
	UINT stringTextureID = m_pTexMgr->GetTextureID(m_pFontObjectList->at(fontID).mInternalTextureName);


	refText.mLineSpacingOffset = lineSpacingOffset;
	refText.mWordSpacingOffset = wordSpacingOffset;

	//use the internal init func of TEXT 
	refText.mFontID = fontID;
	refText.mCharBoundarySizeY= UINT(m_pFontObjectList->at(fontID).mFontSize);
	refText.mCharBoundarySizeX= UINT(refText.mCharBoundarySizeY* m_pFontObjectList->at(fontID).mAspectRatio);

	//update TEXT content
	*(refText.m_pTextContent) = targetString;

	//(the texture has been created for each font
	//texture name in	TexMgr
	std::stringstream tmpTextureName;
	tmpTextureName << "AsciiBitmapTable" << fontID;//texture name generated with font ID
	*(refText.m_pTextureName)= tmpTextureName.str();


	//------------initialize the graphic object of the TEXT------------
	//m_pFatherScene->CreateGraphicObject(*refText.m_pGraphicObj);
	refText.mFunction_InitGraphicObject(boundaryWidth, boundaryHeight, textColor, stringTextureID);


	//bind Text_Dynamic to MGR
	m_pChildTextDynamic->push_back(&refText);
	refText.m_pFatherFontMgr = this;
	refText.SetStatusToBeInitialized();//life cycle

	return m_pChildTextDynamic->size()-1;
}

UINT	 IFontManager::GetFontID(std::string fontName)
{
	for (UINT i = 0;i < m_pFontObjectList->size();i++)
	{
		N_FontObject fontObj = m_pFontObjectList->at(i);
		if (fontName == fontObj.mFontName)
		{
			return i;
		}
	}

	return NOISE_MACRO_INVALID_ID;
}

NVECTOR2 IFontManager::GetFontSize(UINT fontID)
{
	float fontWidth = 0.0f; float fontHeight = 0.0f;
	if (fontID < m_pFontObjectList->size())
	{
		fontHeight = float(m_pFontObjectList->at(fontID).mFontSize);
		fontWidth = fontHeight*m_pFontObjectList->at(fontID).mAspectRatio;
	}
	return NVECTOR2(fontWidth,fontHeight);
}


/************************************************************************
										P R I V A T E
************************************************************************/

BOOL IFontManager::mFunction_InitFreeType()
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

void IFontManager::mFunction_GetBitmapOfChar(N_FontObject& fontObj, wchar_t targetWChar, N_Font_Bitmap & outFontBitmap, NVECTOR4 textColor)
{
	//didn't check fontID here , only be able to get the bitmap
	
	//if user pass the same N_Font_Bitmap several times , repetitive push_back will finally cause errors
	outFontBitmap.bitmapBuffer.clear();

	//Load Glyph from the target Unicode 
	FT_Error ftErr;
	//get corresponding ID of the w_char
	FT_UInt	 glyph_index = FT_Get_Char_Index(fontObj.mFtFace, targetWChar);
	ftErr = FT_Load_Glyph(
		fontObj.mFtFace,
		glyph_index,
		FT_LOAD_DEFAULT);

	//Get the Glyph of char
	FT_Glyph glyph;
	if (FT_Get_Glyph(fontObj.mFtFace->glyph, &glyph))
	{
		DEBUG_MSG1("FreeType:GetGlyph Failed");
		return;
	}

	//convert (and render) to a bitmap buffer
	FT_Render_Glyph(fontObj.mFtFace->glyph, FT_RENDER_MODE_NORMAL);
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	//use the FT_Bitmap's 'buffer' to retrieve data
	FT_Bitmap& charBitmap = bitmap_glyph->bitmap;

	//assign color values to user-defined color buffer ref
	int charWidth = charBitmap.width;
	int charHeight = charBitmap.rows;

	//---------------------CHAR SIZE--------------------
	//after multiple passing firstly started in CreateFont
	//the reference of FontObj finally come here to get bitmap size for each char
	fontObj.mAsciiCharSizeList.push_back(NVECTOR2(float(charWidth), float(charHeight)));

	//output to references
	for (int i = 0;i < charWidth*charHeight;i++)
	{
		if (charBitmap.buffer[i])
		{
			//add colors to user-defined buffer (greyScaleIntensity * color)
			float greyScaleColor = float(charBitmap.buffer[i]) / 256.0f;
			outFontBitmap.bitmapBuffer.push_back(
				NVECTOR4(
					greyScaleColor *textColor.x,
					greyScaleColor* textColor.y,
					greyScaleColor* textColor.z,
					greyScaleColor));
		}
		else
			//current pixel  is black (not in glyph)
		{
			outFontBitmap.bitmapBuffer.push_back(NVECTOR4(0, 0, 0, 0));
		}
	}
	outFontBitmap.height = charHeight;
	outFontBitmap.width = charWidth;
}

void	IFontManager::mFunction_GetBitmapOfString(N_FontObject& fontObj, std::wstring targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, N_Font_Bitmap & outFontBitmap, int wordSpacingOffset, int lineSpacingOffset)
{


	//arbitrarily get a wchar to Get the basic size of char
	N_Font_Bitmap singleCharBitmap;
	UINT CharBoundarySizeY = fontObj.mFontSize;
	UINT CharBoundarySizeX = UINT(CharBoundarySizeY*fontObj.mAspectRatio);


	//control the offset  prevent lines getting to near
	if (lineSpacingOffset <= -int(CharBoundarySizeY) + 2)lineSpacingOffset = CharBoundarySizeY;
	//control the offset to prevent words getting to near
	if (wordSpacingOffset <= -int(CharBoundarySizeX) + 2)wordSpacingOffset = CharBoundarySizeX;

	//a string rectangle must be big enough to contain at least  1 word
	if (CharBoundarySizeX > boundaryWidth)boundaryWidth = CharBoundarySizeX + 1;
	if (CharBoundarySizeY > boundaryHeight)boundaryHeight = CharBoundarySizeY + 1;
	outFontBitmap.bitmapBuffer.resize(boundaryWidth*boundaryHeight, NVECTOR4(0, 0, 0, 0));


	//used to locate the position of current drawing char
	UINT currentCharTopLeftX = 0;
	UINT currentCharTopLeftY = 0;
	int	currentCharAlignmentOffsetY = 0;

	for (UINT i = 0;i < targetString.size();i++)
	{

		//get the bitmap of each char, later combine them into a string in an adequate layout
		mFunction_GetBitmapOfChar(fontObj, targetString.at(i), singleCharBitmap, textColor);

		//we will prevent every word from being incomplete , so goto next line 
		if (currentCharTopLeftX + singleCharBitmap.width + wordSpacingOffset >= boundaryWidth)
		{
			//go to next line
			currentCharTopLeftX = 0;
			currentCharTopLeftY += (CharBoundarySizeY + lineSpacingOffset);
		}

	
		//get alignment offset 
		currentCharAlignmentOffsetY = gFunction_GetCharAlignmentOffsetPixelY(CharBoundarySizeY, singleCharBitmap.height, targetString.at(i));


		UINT outputBitmapPixelCount = outFontBitmap.bitmapBuffer.size();
		//-----------draw a char in global space-------------
		for (UINT localY = 0;localY < singleCharBitmap.height;localY++)
		{
			for (UINT localX = 0;localX < singleCharBitmap.width;localX++)
			{
				

				//pixel index in string bitmap buffer ( in contrast to local single char bitmap buffer)
				//remember to align to the bottom
				UINT globalPixelID =
					(currentCharTopLeftY + currentCharAlignmentOffsetY + localY)*boundaryWidth +
					(currentCharTopLeftX + localX);

				UINT localPixelID = localY* singleCharBitmap.width + localX;

				//check if currently drawing pixel have exceed the lower bound
				if (globalPixelID < outputBitmapPixelCount)
				{
					//alpha !=0 , not background color
					if (singleCharBitmap.bitmapBuffer.at(localPixelID).w != 0)
					{
						outFontBitmap.bitmapBuffer.at(globalPixelID) = singleCharBitmap.bitmapBuffer.at(localPixelID);
					}
				}

			}
		}

		//move to next position
		currentCharTopLeftX += (singleCharBitmap.width + wordSpacingOffset);


	}

	outFontBitmap.width = boundaryWidth;
	outFontBitmap.height = boundaryHeight;
}

inline UINT IFontManager::mFunction_ValidateFontID(UINT fontID)
{
	if (!(fontID >= 0 && fontID < m_pFontObjectList->size()))
	{
		return NOISE_MACRO_INVALID_ID;
	}

	//default return 
	return fontID;
}

UINT IFontManager::mFunction_CreateTexture_AsciiBitmapTable(N_FontObject& fontObj, UINT charWidth, UINT charHeight)
{

	//define the width /height of bitmap Table , Ascii code 0~127
	UINT tablePxWidth = charWidth*NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_COLUMN_COUNT;
	UINT tablePxHeight = charHeight*NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_ROW_COUNT;
	UINT tableRowCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_ROW_COUNT;
	UINT tableColumnCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_COLUMN_COUNT;

	//try to create a new pure color texture to be modified
	UINT stringTextureID = NOISE_MACRO_INVALID_TEXTURE_ID;
	std::stringstream tmpTextureName;
	tmpTextureName << "AsciiBitmapTable" << (m_pFontObjectList->size());//"AsciiBitmapTable"+font ID
	fontObj.mInternalTextureName = tmpTextureName.str();//not same with the public FONT NAME

	//Create a pure color Texture
	stringTextureID = m_pTexMgr->CreatePureColorTexture(
		tmpTextureName.str().c_str(),
		tablePxWidth,
		tablePxHeight,
		NVECTOR4(0, 0, 0, 0),
		TRUE
		);

	//check if texture creation success
	if (stringTextureID == NOISE_MACRO_INVALID_TEXTURE_ID)
	{
		DEBUG_MSG1("CreateFontFromFile : Create Bitmap Table Texture failed!");
		return NOISE_MACRO_INVALID_ID;
	}
	
	//-----Up to now,the texture is still a pure color bitmap-------
	//-----we are gonna write an ASCII bitmap table to it (code 0~127)--
	auto& pixelBuff= m_pTexMgr->m_pTextureObjectList->at(stringTextureID).mPixelBuffer;

	for (UINT rowID = 0;rowID < tableRowCount;rowID++)
	{
		for (UINT colID = 0;colID < tableColumnCount;colID++)
		{

			N_Font_Bitmap tmpFontBitmap;
			mFunction_GetBitmapOfChar(fontObj, rowID*tableColumnCount+colID, tmpFontBitmap, NVECTOR4(1.0f, 0, 0, 1.0f));
			for (UINT localY = 0;localY < charHeight;localY++)
			{
				for (UINT localX = 0;localX < charWidth;localX++)
				{
					//...the size of  char bitmap might didn't match the size of the boundary rect(for 1 char)
					if (localX < tmpFontBitmap.width && localY < tmpFontBitmap.height)
					{
						//copy every char bitmap to the global char bitmap table
						UINT currentPixelID = (rowID*charHeight + localY)*tablePxWidth + colID*charWidth + localX;
						pixelBuff.at(currentPixelID) = tmpFontBitmap.bitmapBuffer.at(localY*tmpFontBitmap.width + localX);
					}
				}
			}

		}
	}
	//update a texture to Graphic Memory
	m_pTexMgr->UpdateTextureDataToGraphicMemory(stringTextureID);


	return stringTextureID;
}

