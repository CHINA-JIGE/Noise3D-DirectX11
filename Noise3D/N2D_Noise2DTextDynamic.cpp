/***********************************************************************

								cpp£ºNoise2DText_Dynamic 

************************************************************************/
#include "Noise3D.h"

Noise2DTextDynamic::Noise2DTextDynamic()
{
	mWordSpacingOffset = 0;
	mLineSpacingOffset = 0;
	m_pTextureName = new std::string;
	m_pTextContent	= new std::string;
	mIsTextContentChanged=TRUE;
	mIsSizeChanged=TRUE;
}

void Noise2DTextDynamic::SetWidth(float w)
{
	if (w != GetWidth())
	{
		Noise2DBasicContainerInfo::SetWidth(w);
		mIsSizeChanged = TRUE;
	}
}

void Noise2DTextDynamic::SetHeight(float h)
{
	if (h != GetHeight())
	{
		Noise2DBasicContainerInfo::SetHeight(h);
		mIsSizeChanged = TRUE;
	}
};


void Noise2DTextDynamic::Destroy()
{
	m_pGraphicObj->SelfDestruction();
};

void Noise2DTextDynamic::SetFont(UINT fontID)
{
	if (m_pFatherFontMgr->mFunction_ValidateFontID(fontID) != NOISE_MACRO_INVALID_ID)
	{
		mFontID = fontID;
		auto fontObj =m_pFatherFontMgr->m_pFontObjectList->at(fontID);
		
		//boundary size of 1 Char
		mCharBoundarySizeY = UINT(fontObj.mFontSize);
		mCharBoundarySizeX = UINT(mCharBoundarySizeY*fontObj.mAspectRatio);

		//update Texture ID using name "AsciiBitmapTable" + number
		mStringTextureID = m_pFatherFontMgr->m_pTexMgr->GetTextureID(fontObj.mInternalTextureName);

	}
}

UINT Noise2DTextDynamic::GetFontID()
{
	return mFontID;
}

NVECTOR2 Noise2DTextDynamic::GetFontSize(UINT fontID)
{
	if (!IsInitialized())return NVECTOR2(0, 0);
	return m_pFatherFontMgr->GetFontSize(fontID);
};


void Noise2DTextDynamic::SetTextAscii(std::string newText)
{
	//std::hash<std::string>() can hash a key
	//if (std::hash<std::string>()(newText) != std::hash<std::string>()(*m_pTextContent))
	if(newText!=*m_pTextContent)
	{
		*m_pTextContent = newText;
		mIsTextContentChanged = TRUE;
	}
}

void Noise2DTextDynamic::GetTextAscii(std::string & outString)
{
	outString = *m_pTextContent;
};


void Noise2DTextDynamic::SetLineSpacingOffset(int offset)
{
	if (offset > -int(mCharBoundarySizeY))mLineSpacingOffset = offset;
}

int Noise2DTextDynamic::GetLineSpacingOffset()
{
	return mLineSpacingOffset;
};


void Noise2DTextDynamic::SetWordSpacingOffset(int offset)
{
	if (offset > -int(mCharBoundarySizeX))mWordSpacingOffset = offset;
}

int Noise2DTextDynamic::GetWordSpacingOffset()
{
	return mWordSpacingOffset;
}

NVECTOR2 Noise2DTextDynamic::GetWordLocalPosOffset(UINT wordIndex)
{
	if (!NoiseClassLifeCycle::IsInitialized())
	{
		DEBUG_MSG1("Text Dynamic is not initialized!!");
		return NVECTOR2(0, 0);
	}//ensure font mgr has been initialized

	//There is some overlapped code in mFunc_UpdateGraphicObj()
	std::string str;
	GetTextAscii(str);

	UINT stringBoundaryWidth = UINT(Noise2DBasicContainerInfo::GetWidth());
	UINT stringBoundaryHeight = UINT(Noise2DBasicContainerInfo::GetHeight());

	NVECTOR2	posTopLeftOffset(0, 0);

	//clamp wordIndex
	for (UINT i = 0;i <	(wordIndex<str.size()?wordIndex:str.size()); i++)
	{
		NVECTOR2 realCharBitmapSize = GetWordRealSize(m_pTextContent->at(i));

		//sum up widths and heights of words to derive pos offset
		posTopLeftOffset.x += (realCharBitmapSize.x + mWordSpacingOffset);

		//if the right side of current char has exceeded the right text boundary
		if (posTopLeftOffset.x + realCharBitmapSize.x >= stringBoundaryWidth)
		{
			//cursor move to next line
			posTopLeftOffset.y += (mCharBoundarySizeY + mLineSpacingOffset);
			posTopLeftOffset.x = 0.0f;
		}
	}

	return posTopLeftOffset;

};

inline NVECTOR2 Noise2DTextDynamic::GetWordRealSize(UINT wordIndex)
{
	if (!NoiseClassLifeCycle::IsInitialized())
	{
		DEBUG_MSG1("Text Dynamic is not initialized!!");
		return NVECTOR2(0, 0);//ensure font mgr has been initialized
	}

	NVECTOR2 realCharBitmapPixelSize = m_pFatherFontMgr->m_pFontObjectList->at(mFontID).mAsciiCharSizeList.at(wordIndex);
	return realCharBitmapPixelSize;
};


/************************************************************************
										P R I V A T E
************************************************************************/
void Noise2DTextDynamic::mFunction_InitGraphicObject(UINT pxWidth, UINT pxHeight, NVECTOR4 color, UINT texID)
{

	m_pGraphicObj->AddRectangle(
		NVECTOR2(float(pxWidth) / 2.0f, float(pxHeight) / 2.0f),
		float(pxWidth),
		float(pxHeight),
		color,
		texID
		);

	*m_pTextColor = color;
	*m_pTextGlowColor = color;
	Noise2DBasicContainerInfo::SetWidth(float(pxWidth));
	Noise2DBasicContainerInfo::SetHeight(float(pxHeight));
};

void  Noise2DTextDynamic::mFunction_UpdateGraphicObject()//call by Renderer:AddObjectToRenderList
{
	if (!NoiseClassLifeCycle::IsInitialized())return;

	mFontID = m_pFatherFontMgr->mFunction_ValidateFontID(mFontID);
	//.....we must clear the graphic objects 
	if (mFontID == NOISE_MACRO_INVALID_ID)
	{
		for (UINT i = 0;i< m_pGraphicObj->GetRectCount();i++)
		{
			//delete from pos '0'
			m_pGraphicObj->DeleteRectangle(0);
		}
		return;
	};

	//if no need to change ,then skip unnecessary UPDATE (especially to optimize UpdateSubresource)
	//those SetRectangle/SetXXX might cause GraphicObject to update ,then data must be updated to GPU
	NVECTOR2 aaa = m_pGraphicObj->GetBasePosOffset();
	NVECTOR2 bbb = GetTopLeft();
	if ( mIsSizeChanged == FALSE
		&& mIsTextContentChanged == FALSE
		 && m_pGraphicObj->GetBasePosOffset()==GetTopLeft())
		return;


	UINT stringBoundaryWidth = UINT(Noise2DBasicContainerInfo::GetWidth());
	UINT stringBoundaryHeight = UINT(Noise2DBasicContainerInfo::GetHeight());
	const UINT tableRowCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_ROW_COUNT;
	const UINT tableColumnCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_COLUMN_COUNT;
	//texcoord offset for 1 char bitmap ( char bitmap might be much smaller than the boundary)
	float charBoundaryTexcoordOffsetX = 1 / float(tableColumnCount);
	float charBoundaryTexcoordOffsetY = 1 / float(tableRowCount);

	//in case other font has been deleted 


	UINT stringCharCount = m_pTextContent->size();

	//------------------------------------------------
	//now , we will determine whether to add/set/delete rectangles according to 
	//newly updated TextContent

#pragma region AdjustTheCountOfRects

	//1.adjust rectangles count to fit the chars' count of new Text
	UINT currentRectCountInGraphicObj = m_pGraphicObj->GetRectCount();
	if (stringCharCount>currentRectCountInGraphicObj)
	{
		for (UINT i = 0;i < stringCharCount - currentRectCountInGraphicObj;i++)
		{
			//delete redundant
			NVECTOR2 tmpZeroVec2(0, 0);NVECTOR4 tmpZeroVec4(0, 0, 0, 0);
			m_pGraphicObj->AddRectangle(tmpZeroVec2, tmpZeroVec2, tmpZeroVec4, mStringTextureID);
		}
	}
	else
	{		
		//delete redundant rectangles
		for (UINT i = 0;i < currentRectCountInGraphicObj - stringCharCount;i++)
		{
			//delete elements at this position
			m_pGraphicObj->DeleteRectangle(stringCharCount );
		}
	}

#pragma endregion AdjustTheCountOfRects

	//1.adjust rectangles count to fit the chars' count of new Text
	//m_pGraphicObj->AdjustRectangleCount(stringCharCount);


	//2. update (set) rectangles to its new condition, and we store 2 string to avoid unnecessary 
	//texcoord update (char bitmap udpate)
	//like a cursor moving around
	NVECTOR2	posGeneralOffset(0, 0);
	NVECTOR2	posAlignmentOffset(0, 0);//some special characters need special alignment like 'g' 'p' 'q'
	for (UINT i = 0;i < stringCharCount;i++)
	{

		//chars which are at the corresponding position (if one string is longer than the other
		char currentChar = m_pTextContent->at(i);//(i < m_pTextContent->size()) ? m_pTextContent->at(i) : 0;


#pragma region UpdateTexcoord
		//TEXCOORD modification
		//see if we should re-locate the texcoord of current rectangle (to see if the char changes?)

		NVECTOR2 newTexCoordTopLeft(0, 0);
		NVECTOR2 newTexCoordBottomRight(0, 0);

		//lookup a char bitmap in the table using texcoord (table size: tableRowCount * tableColumnCount)
		newTexCoordTopLeft.x = (currentChar % tableColumnCount) / float(tableColumnCount);
		newTexCoordTopLeft.y = (currentChar / tableColumnCount) / float(tableRowCount);
		newTexCoordBottomRight.x = newTexCoordTopLeft.x + charBoundaryTexcoordOffsetX;
		newTexCoordBottomRight.y = newTexCoordTopLeft.y + charBoundaryTexcoordOffsetY;
	

		//update texcoord of current rect
		m_pGraphicObj->SetRectangleTexCoord(i, newTexCoordTopLeft, newTexCoordBottomRight);

#pragma endregion UpdateTexcoord


		NVECTOR2 realCharBitmapPixelSize = m_pFatherFontMgr->m_pFontObjectList->at(mFontID).mAsciiCharSizeList.at(currentChar);


#pragma region UpdatePositionOfSubRects

		//a base OFFSET to be added to vertices
		m_pGraphicObj->SetBasePosOffset(GetTopLeft());

		//calculate position of Rects (relative to the graphic object container)
		posAlignmentOffset.x = 0;
		posAlignmentOffset.y = (float)gFunction_GetCharAlignmentOffsetPixelY(mCharBoundarySizeY, UINT(realCharBitmapPixelSize.y), currentChar);

		NVECTOR2 tmpRectTopLeft(0, 0);
		NVECTOR2 tmpRectBottomRight(0, 0);
		//tmpRectTopLeft = Noise2DBasicContainerInfo::GetTopLeft() + posGeneralOffset + posAlignmentOffset;
		tmpRectTopLeft =  posGeneralOffset + posAlignmentOffset;
		tmpRectBottomRight = tmpRectTopLeft + NVECTOR2(float(mCharBoundarySizeX), float(mCharBoundarySizeY));
		
		//COLLAPSE the rectangle into ONE POINT to make it invisible if this rectangle
		//is outside the boundary of Text
		if (GetTopLeft().y+ tmpRectTopLeft.y+realCharBitmapPixelSize.y >GetBottomRight().y)
		{
			//collapse
			m_pGraphicObj->SetRectangle(
				i,
				NVECTOR2(0, 0),
				NVECTOR2(0, 0),
				Noise2DBasicContainerInfo::GetBasicColor(),
				mStringTextureID
				);
		}
		else
		{
			//common set
			m_pGraphicObj->SetRectangle(
				i,
				tmpRectTopLeft,
				tmpRectBottomRight,
				Noise2DBasicContainerInfo::GetBasicColor(),
				mStringTextureID
				);
		}

		//rectangle depth,used for 2D overlapping
		m_pGraphicObj->SetRectangleDepth(i, mPosZ);


#pragma endregion UpdatePositionOfSubRects


		//update internal position offset ( to move the texcoord cursor of "TopLeft")
		posGeneralOffset.x += (realCharBitmapPixelSize.x+mWordSpacingOffset);

		//if the right side of current char has exceeded the right text boundary
 		if (posGeneralOffset.x +realCharBitmapPixelSize.x >= stringBoundaryWidth)
		{
			//cursor move to next line
			posGeneralOffset.y +=( mCharBoundarySizeY+mLineSpacingOffset);
			posGeneralOffset.x = 0.0f;
		}

	}//1 char update finished


	mIsSizeChanged = FALSE;
	mIsTextContentChanged = FALSE;
//all chars update finished.
};

