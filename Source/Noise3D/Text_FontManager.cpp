/***********************************************************************

								class£ºNoiseFontLoader
		description : load /process /provide font texture(pixel matrix)
		

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

N_FontObject::N_FontObject()
{
	mFontSize = 10;
	mAspectRatio= 1.0f / 1.414f;
}

N_FontObject::~N_FontObject()
{
	//FT_Done_Face(mFtFace);
	mAsciiCharSizeList.clear();
};


IFontManager::IFontManager():
	IFactory<N_FontObject>(5000),
	IFactory<IDynamicText>(100000),
	IFactory<IStaticText>(100000)
{
	mIsFTInitialized = false;
	m_FTLibrary		= nullptr;
}

IFontManager::~IFontManager()
{
	//FT faces will be deleted, then erase font objects
	IFactory<N_FontObject>::DestroyAllObject();
	FT_Done_FreeType(m_FTLibrary);

	//don't need these 2 GC statement, Scene garbage collection will do that
	//m_pTexMgr->DeleteAllTexture();//font texture
	//m_pGraphicObjMgr->DestroyAllGraphicObj();//text containers

	IFactory<IStaticText>::DestroyAllObject();
	IFactory<IDynamicText>::DestroyAllObject();
}

bool	 IFontManager::CreateFontFromFile(NFilePath filePath, N_UID fontName, UINT fontSize, float fontAspectRatio)
{

	//open font file (.ttf ,etc.)
	std::fstream tmpFile(filePath);
	if (!tmpFile.is_open())
	{
		ERROR_MSG("CreateFont : file path not exist... Path:" + filePath);
		return false;
	}

	//font name must not be used
	if(IFactory<N_FontObject>::FindUid(fontName)==true)
	{
			ERROR_MSG("CreateFont : Font Name has been used!");
			return false;
	}


	//temporary font object
	N_FontObject tmpFontObj;//too many thing to pass down, so pass a ref of fontObj might be a good idea

	//Create a FT_Face (to store font/char  info)
	FT_Error ftCreateNewFaceErr = FT_New_Face(
		m_FTLibrary,
		filePath.c_str(),
		0,		//which style	,	regular/italic/bold/italic_bold (maybe available)
		&tmpFontObj.mFtFace);

	if (ftCreateNewFaceErr)
	{
		ERROR_MSG("FontMgr : Create Font failed!");
		return false;
	}
	else
	{
		//
		mIsFTInitialized = true;
	};

	FT_Set_Pixel_Sizes(tmpFontObj.mFtFace, UINT(fontSize / 1.414), fontSize);
	//font size should be multiplied by 64 .... I don't know why but it's how it works
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
	bool createBitmapTableSucceed = mFunction_CreateTexture_AsciiBitmapTable(
		tmpFontObj,
		fontName,
		UINT(fontSize*fontAspectRatio),
		fontSize);//bitmap size (height) for 1 ascii char

	if (createBitmapTableSucceed == false)
	{
		ERROR_MSG("CreateFont : create bitmap table failed!!");
		return false;
	}

	//create a font obj and initialize
	N_FontObject* pFontObj = IFactory<N_FontObject>::CreateObject(fontName);
	pFontObj->mFontSize = fontSize;
	pFontObj->mAspectRatio = fontAspectRatio;
	pFontObj->mFtFace = tmpFontObj.mFtFace;
	pFontObj->mAsciiCharSizeList = std::move(tmpFontObj.mAsciiCharSizeList);
	pFontObj->mInternalTextureName = std::move(tmpFontObj.mInternalTextureName);

	tmpFile.close();

	return true;
}

bool	IFontManager::SetFontSize(N_UID fontName, UINT  fontSize)
{
	if (IFactory<N_FontObject>::FindUid(fontName)== false)
	{
		if (fontSize < 4)fontSize = 4;
		auto pFontObj = IFactory<N_FontObject>::GetObjectPtr(fontName);
		pFontObj->mFontSize = fontSize;
		//font size should be multiplied by 64 .... I don't know why but it's how it works
		//FT_Set_Pixel_Sizes(tmpFontObj.mFtFace, UINT(fontSize / 1.414), fontSize);
		FT_Set_Char_Size(pFontObj->mFtFace, UINT(fontSize / 1.414) << 6, fontSize << 6, 72, 72);


		return true;
	}
	return false;
}

bool IFontManager::IsFontExisted(N_UID fontName)
{
	return IFactory<N_FontObject>::FindUid(fontName);
}

IStaticText*	 IFontManager::CreateStaticTextA(N_UID fontName, N_UID textObjectName, std::string contentString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset)
{
	//conver WString to AsciiString
	std::wstring tmpWString;
	tmpWString.assign(contentString.begin(), contentString.end());
	IStaticText* pText = CreateStaticTextW(fontName, textObjectName,tmpWString, boundaryWidth, boundaryHeight, textColor, wordSpacingOffset, lineSpacingOffset);
	return pText;
};

IStaticText*	 IFontManager::CreateStaticTextW(N_UID fontName, N_UID textObjectName, std::wstring contentString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset)
{
	//check fontName if it repeats
	if (IFactory<N_FontObject>::FindUid(fontName) == false)
	{
		ERROR_MSG("CreateStaticTextW:Font Name Invalid!!");
		return nullptr;
	}

	if (IFactory<IStaticText>::FindUid(textObjectName) == true)
	{
		ERROR_MSG("CreateStaticTextW: static Text UID existed!");
		return nullptr;
	}


	//texture name in TexMgr
	N_UID tmpTextureName="Internal_Static_Tex" +textObjectName;

	//Create a pure color Texture
	ITexture* pTexture= m_pTexMgr->CreatePureColorTexture(
		tmpTextureName,
		boundaryWidth,
		boundaryHeight,
		NVECTOR4(0, 0, 0, 0),
		true
		);

	//check if texture creation success
	if (pTexture==nullptr)
	{
		ERROR_MSG("CreateStaticTextW : Create Bitmap Table Texture failed!");
		return nullptr;
	}

	//now get the whole bitmap from user-input string
	N_Font_Bitmap tmpFontBitmap;
	mFunction_GetBitmapOfWString(
		*IFactory<N_FontObject>::GetObjectPtr(fontName),//font ID validated
		contentString,
		boundaryWidth,
		boundaryHeight,
		textColor,
		tmpFontBitmap,
		0,
		0);

	//copy bitmap to texture 
	pTexture->SetPixelArray(std::move(tmpFontBitmap.bitmapBuffer));

	//update a texture in the identity of FONT MGR (which match the Required Access Permission)
	bool UpdateToGMSuccess = false;
	UpdateToGMSuccess = pTexture->UpdateToVideoMemory();
	if (!UpdateToGMSuccess)
	{
		ERROR_MSG("CreateStaticTextW : Create Text Bitmap failed!!");
		m_pTexMgr->DeleteTexture(tmpTextureName);
		return nullptr;
	}


	//------------initialize the graphic object of the TEXT------------
	IStaticText* pText = IFactory<IStaticText>::CreateObject(textObjectName);

	//create internal GraphicObj for static Text
	N_UID tmpGraphicObjectName = "Internal_Static_GObj" + textObjectName;
	IGraphicObject* pObj=m_pGraphicObjMgr->CreateGraphicObj(tmpGraphicObjectName);
	pText->mFunction_InitGraphicObject(pObj,boundaryWidth, boundaryHeight, textColor, tmpTextureName);


	//....only after all init work was done can we bind mgr/object together

	pText->mTextureName=tmpTextureName;
	pText->mFontName = fontName;

	return pText;
}

IDynamicText*	 IFontManager::CreateDynamicTextA(N_UID fontName, N_UID textObjectName, std::string contentString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, int wordSpacingOffset, int lineSpacingOffset)
{
	//dynamic text use bitmap table & texture coordinate to  render text

	//check fontName if it existed
	if (IFactory<N_FontObject>::FindUid(fontName) == false)
	{
		ERROR_MSG("WARN : CreateDynamicTextA:Font Name Invalid!!");
		return nullptr;
	}


	if (IFactory<IDynamicText>::FindUid(textObjectName) == true)
	{
		ERROR_MSG("CreateDynamicTextA: dynamic Text UID existed!");
		return nullptr;
	}

	//create a dynamic text
	IDynamicText* pText = IFactory<IDynamicText>::CreateObject(textObjectName);
	if (pText == nullptr)
	{
		ERROR_MSG("CreateDynamicTextA: text ptr creation failed.");
		return nullptr;
	}

	pText->mLineSpacingOffset = lineSpacingOffset;
	pText->mWordSpacingOffset = wordSpacingOffset;

	//use the internal init func of TEXT 
	pText->mFontName = fontName;
	pText->mCharBoundarySizeY= UINT(IFactory<N_FontObject>::GetObjectPtr(fontName)->mFontSize);
	pText->mCharBoundarySizeX= UINT(pText->mCharBoundarySizeY* IFactory<N_FontObject>::GetObjectPtr(fontName)->mAspectRatio);

	//update TEXT content
	pText->mTextContent = contentString;

	//(the texture has been created for each font
	//texture name in	TexMgr
	std::string tmpTextureName= "AsciiBitmapTable" + fontName;//texture name generated with font ID
	pText->mTextureName= tmpTextureName;


	//------------initialize the graphic object of the TEXT------------
	N_UID tmpGraphicObjectName = "Internal_Dyn_GObj" + textObjectName;
	IGraphicObject* pObj = m_pGraphicObjMgr->CreateGraphicObj(tmpGraphicObjectName);
	pText->mFunction_InitGraphicObject(pObj,boundaryWidth, boundaryHeight, textColor, tmpTextureName);


	return pText;
}

NVECTOR2 IFontManager::GetFontSize(N_UID fontName)
{
	float fontWidth = 0.0f; float fontHeight = 0.0f;
	if (IFactory<N_FontObject>::FindUid(fontName)==true)
	{
		auto pObj = IFactory<N_FontObject>::GetObjectPtr(fontName);
		fontHeight = float(pObj->mFontSize);
		fontWidth = fontHeight*pObj->mAspectRatio;
	}
	return NVECTOR2(fontWidth,fontHeight);
}

bool IFontManager::DeleteFont(N_UID fontName)
{
	return IFactory<N_FontObject>::DestroyObject(fontName);
}

void IFontManager::DeleteAllFont()
{
	IFactory<N_FontObject>::DestroyAllObject();
}

bool IFontManager::DeleteStaticText(N_UID textName)
{
	// delete internal graphicObj and corresponding texture
	IStaticText* pText=  IFactory<IStaticText>::GetObjectPtr(textName);
	if (pText != nullptr)
	{
		m_pGraphicObjMgr->DestroyGraphicObj(pText->m_pGraphicObj);
		m_pTexMgr->DeleteTexture(pText->mTextureName);//the appearance of text is expressed as a texture
		IFactory<IStaticText>::DestroyObject(textName);
		return true;
	}
	else
	{
		return false;
	}
}

bool IFontManager::DeleteStaticText(IStaticText * pText)
{
	// delete internal graphicObj
	if (pText != nullptr)
	{
		m_pGraphicObjMgr->DestroyGraphicObj(pText->m_pGraphicObj);
		m_pTexMgr->DeleteTexture(pText->mTextureName);//the appearance of text is expressed as a texture
		IFactory<IStaticText>::DestroyObject(pText);
		return true;
	}
	else
	{
		return false;
	}
}

bool IFontManager::DeleteDynamicText(N_UID textName)
{
	// delete internal graphicObj and corresponding texture
	IDynamicText* pText = IFactory<IDynamicText>::GetObjectPtr(textName);
	if (pText != nullptr)
	{
		m_pGraphicObjMgr->DestroyGraphicObj(pText->m_pGraphicObj);
		IFactory<IDynamicText>::DestroyObject(textName);
		return true;
	}
	else
	{
		return false;
	}
}

bool IFontManager::DeleteDynamicText(IDynamicText * pText)
{
	// delete internal graphicObj and corresponding texture
	if (pText != nullptr)
	{
		m_pGraphicObjMgr->DestroyGraphicObj(pText->m_pGraphicObj);
		m_pTexMgr->DeleteTexture(pText->mTextureName);//the appearance of text is expressed as a texture
		IFactory<IDynamicText>::DestroyObject(pText);
		return true;
	}
	else
	{
		return false;
	}
}

void IFontManager::DeleteAllTexts()
{
	//all texts own a private GObject
	m_pGraphicObjMgr->DestroyAllGraphicObj();

	for (UINT i = 0;i < IFactory<IStaticText>::GetObjectCount();++i)
	{
		//static text didn't use a public font texture(ascii bitmap table),instead,
		//a new texture is created for each static text
		IStaticText* pText = IFactory<IStaticText>::GetObjectPtr(i);
		m_pTexMgr->DeleteTexture(pText->mTextureName);//the appearance of text is expressed as a texture
		IFactory<IStaticText>::DestroyObject(pText);
	}

	IFactory<IDynamicText>::DestroyAllObject();

}

void IFontManager::DeleteAllFonts()
{
	for (UINT i = 0;i < IFactory<N_FontObject>::GetObjectCount();++i)
	{
		N_FontObject* pFontObj = IFactory<N_FontObject>::GetObjectPtr(i);
		m_pTexMgr->DeleteTexture(pFontObj->mInternalTextureName);
	}
	IFactory<N_FontObject>::DestroyAllObject();

}


/************************************************************************
										P R I V A T E
************************************************************************/

bool IFontManager::mFunction_Init(ITextureManager* in_created_pTexMgr, IGraphicObjectManager* in_created_pGObjMgr)
{
	//Init FreeType Library
	FT_Error ftInitError = FT_Init_FreeType(&m_FTLibrary);

	if (ftInitError)
	{
		ERROR_MSG("FontMgr: FreeType init failed!");
		m_FTLibrary = nullptr;
		return false;
	}
	else
	{
		if (in_created_pGObjMgr != nullptr && in_created_pGObjMgr != nullptr)
		{
			m_pGraphicObjMgr = in_created_pGObjMgr;
			m_pTexMgr = in_created_pTexMgr;
			mIsFTInitialized = true;
			return true;
		}
		else
		{
			mIsFTInitialized = false;
			ERROR_MSG("Font Mgr: internal TexMgr / GraphicObjectManager init failed");
			return false;
		}


	};

	return true;
}

void IFontManager::mFunction_GetBitmapOfWChar(N_FontObject& fontObj, wchar_t targetWChar, N_Font_Bitmap & outFontBitmap, NVECTOR4 textColor)
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
		ERROR_MSG("FreeType:GetGlyph Failed");
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

	for (int i = 0;i < charWidth*charHeight;i++)
	{
		if (charBitmap.buffer[i])
		{
			//add colors to user-defined buffer (greyScaleIntensity * color)
			float greyScaleColor = float(charBitmap.buffer[i]) / 256.0f;
			outFontBitmap.bitmapBuffer.push_back(
				NColor4u(NVECTOR4(
					greyScaleColor *textColor.x,
					greyScaleColor* textColor.y,
					greyScaleColor* textColor.z,
					greyScaleColor)));
		}
		else
			//current pixel  is black (not in glyph)
		{
			outFontBitmap.bitmapBuffer.push_back(NColor4u(0, 0, 0, 0));
		}
	}
	outFontBitmap.height = charHeight;
	outFontBitmap.width = charWidth;
}

void	IFontManager::mFunction_GetBitmapOfWString(N_FontObject& fontObj, std::wstring targetString, UINT boundaryWidth, UINT boundaryHeight, NVECTOR4 textColor, N_Font_Bitmap & outFontBitmap, int wordSpacingOffset, int lineSpacingOffset)
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
		mFunction_GetBitmapOfWChar(fontObj, targetString.at(i), singleCharBitmap, textColor);

		//we will prevent every word from being incompletely displayed. (thus last word will be put in next line)
		if (currentCharTopLeftX + singleCharBitmap.width + wordSpacingOffset >= boundaryWidth)
		{
			//go to next line
			currentCharTopLeftX = 0;
			currentCharTopLeftY += (CharBoundarySizeY + lineSpacingOffset);
		}

	
		//get alignment offset 
		currentCharAlignmentOffsetY = gFunc_GetCharAlignmentOffsetPixelY(CharBoundarySizeY, singleCharBitmap.height, targetString.at(i));


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
					if (singleCharBitmap.bitmapBuffer.at(localPixelID).a != 0)
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

bool IFontManager::mFunction_CreateTexture_AsciiBitmapTable(N_FontObject& fontObj,std::string fontName, UINT charWidth, UINT charHeight)
{
	//define the width /height of bitmap Table , Ascii code 0~127
	UINT tablePxWidth = charWidth*NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_COLUMN_COUNT;
	UINT tablePxHeight = charHeight*NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_ROW_COUNT;
	UINT tableRowCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_ROW_COUNT;
	UINT tableColumnCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_COLUMN_COUNT;

	//try to create a new pure color texture to be modified
	fontObj.mInternalTextureName = "AsciiBitmapTable" + fontName;//not same with the public FONT NAME

	//Create a pure color Texture
	 ITexture* pTexture = m_pTexMgr->CreatePureColorTexture(
		 fontObj.mInternalTextureName,
		tablePxWidth,
		tablePxHeight,
		NVECTOR4(0, 0, 0, 0),
		true
		);

	//check if texture creation success
	if (pTexture==nullptr)
	{
		ERROR_MSG("CreateFontFromFile : Create Bitmap Table Texture failed!");
		return false;
	}
	
	//-----Up to now,the texture is still a pure color bitmap-------
	//-----we are gonna write an ASCII bitmap table to it (code 0~127)--
	std::vector<NColor4u> pixelBuff(tablePxWidth*tablePxHeight);

	for (UINT rowID = 0;rowID < tableRowCount;rowID++)
	{
		for (UINT colID = 0;colID < tableColumnCount;colID++)
		{

			N_Font_Bitmap tmpFontBitmap;
			mFunction_GetBitmapOfWChar(fontObj,rowID*tableColumnCount+colID, tmpFontBitmap, NVECTOR4(1.0f, 0, 0, 1.0f));
			
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

	pTexture->SetPixelArray(pixelBuff);
	//update a texture to Graphic Memory
	pTexture->UpdateToVideoMemory();

	return true;
}

