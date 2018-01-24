/***********************************************************************

								cpp£ºNoise2DText_Dynamic 

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;

IDynamicText::IDynamicText():
	mWordSpacingOffset(0),
	mLineSpacingOffset(0),
	mIsTextContentChanged(true),
	mIsSizeChanged(true)
{
}

IDynamicText::~IDynamicText()
{
}

void IDynamicText::SetWidth(float w)
{
	if (w != GetWidth())
	{
		IBasicContainerInfo::SetWidth(w);
		mIsSizeChanged = true;
	}
}

void IDynamicText::SetHeight(float h)
{
	if (h != GetHeight())
	{
		IBasicContainerInfo::SetHeight(h);
		mIsSizeChanged = true;
	}
};


void IDynamicText::SetFont(N_UID fontName)
{
	IFontManager* pFontMgr = GetScene()->GetFontMgr();

	if (pFontMgr->IsFontExisted(fontName)==true)
	{
		mFontName = fontName;
		N_FontObject* pfontObj = pFontMgr->IFactory< N_FontObject>::GetObjectPtr(fontName);
		
		//boundary size of 1 Char
		mCharBoundarySizeY = UINT(pfontObj->mFontSize);
		mCharBoundarySizeX = UINT(mCharBoundarySizeY*pfontObj->mAspectRatio);

		//update Texture ID using name "AsciiBitmapTable" + fontName
		mTextureName = pfontObj->mInternalTextureName;

	}
}

N_UID IDynamicText::GetFontName()
{
	return mFontName;
}

NVECTOR2 IDynamicText::GetFontSize(UINT fontID)
{
	IFontManager* pFontMgr = GetScene()->GetFontMgr();
	return pFontMgr->GetFontSize(mFontName);
};


void IDynamicText::SetTextAscii(const std::string& newText)
{
	//std::hash<std::string>() can hash a key
	//if (std::hash<std::string>()(newText) != std::hash<std::string>()(*m_pTextContent))
	if(newText!=mTextContent)
	{
		mTextContent = newText;
		mIsTextContentChanged = true;
	}
}

void IDynamicText::GetTextAscii(std::string & outString)
{
	outString = mTextContent;
};


void IDynamicText::SetLineSpacingOffset(int offset)
{
	if (offset > -int(mCharBoundarySizeY))mLineSpacingOffset = offset;
}

int IDynamicText::GetLineSpacingOffset()
{
	return mLineSpacingOffset;
};


void IDynamicText::SetWordSpacingOffset(int offset)
{
	if (offset > -int(mCharBoundarySizeX))mWordSpacingOffset = offset;
}

int IDynamicText::GetWordSpacingOffset()
{
	return mWordSpacingOffset;
}

//pixel coordinate , position offset of specific word
NVECTOR2 IDynamicText::GetWordLocalPosOffset(UINT wordIndex)
{

	//There is some overlapped code in mFunc_UpdateGraphicObj()
	std::string str;
	GetTextAscii(str);

	UINT stringBoundaryWidth = UINT(IBasicContainerInfo::GetWidth());
	UINT stringBoundaryHeight = UINT(IBasicContainerInfo::GetHeight());

	NVECTOR2	posTopLeftOffset(0, 0);

	//clamp wordIndex
	for (UINT i = 0;i <	(wordIndex<str.size()?wordIndex:str.size()); i++)
	{
		NVECTOR2 realCharBitmapSize = GetWordRealSize(mTextContent.at(i));

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

inline NVECTOR2 IDynamicText::GetWordRealSize(UINT wordIndex)
{
	IFontManager* pFontMgr = GetScene()->GetFontMgr();
	NVECTOR2 realCharBitmapPixelSize = pFontMgr->IFactory<N_FontObject>::GetObjectPtr(mFontName)->mAsciiCharSizeList.at(wordIndex);
	return realCharBitmapPixelSize;
};


/************************************************************************
										P R I V A T E
************************************************************************/
void IDynamicText::mFunction_InitGraphicObject(IGraphicObject* pCreatedObj,UINT pxWidth, UINT pxHeight, NVECTOR4 color, N_UID texName)
{
	m_pGraphicObj = pCreatedObj;

	m_pGraphicObj->AddRectangle(
		NVECTOR2(float(pxWidth) / 2.0f, float(pxHeight) / 2.0f),
		float(pxWidth),
		float(pxHeight),
		color,
		texName
		);

	*m_pTextColor = color;
	*m_pTextGlowColor = color;
	IBasicContainerInfo::SetWidth(float(pxWidth));
	IBasicContainerInfo::SetHeight(float(pxHeight));
};

void  IDynamicText::mFunction_UpdateGraphicObject()//call by Renderer:AddObjectToRenderList
{
	IFontManager* pFontMgr = GetScene()->GetFontMgr();

	//if font is invalid (deleted??), we must clear the graphic objects 
	if (pFontMgr->IsFontExisted(mFontName)== false)
	{
		m_pGraphicObj->DeleteRectangle(0, m_pGraphicObj->GetRectCount()-1);
		return;
	};

	//if no need to change ,then skip unnecessary UPDATE (especially to optimize UpdateSubresource)
	//those SetRectangle/SetXXX might cause GraphicObject to update ,then data must be updated to GPU
	NVECTOR2 aaa = m_pGraphicObj->GetBasePosOffset();
	NVECTOR2 bbb = GetTopLeft();
	if ( mIsSizeChanged == false
		&& mIsTextContentChanged == false
		 && m_pGraphicObj->GetBasePosOffset()==GetTopLeft())
		return;


	UINT stringBoundaryWidth = UINT(IBasicContainerInfo::GetWidth());
	UINT stringBoundaryHeight = UINT(IBasicContainerInfo::GetHeight());
	const UINT tableRowCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_ROW_COUNT;
	const UINT tableColumnCount = NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_COLUMN_COUNT;
	//texcoord offset for 1 char bitmap ( char bitmap might be much smaller than the boundary)
	float charBoundaryTexcoordOffsetX = 1 / float(tableColumnCount);
	float charBoundaryTexcoordOffsetY = 1 / float(tableRowCount);

	//in case other font has been deleted 


	UINT stringCharCount = mTextContent.size();

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
			m_pGraphicObj->AddRectangle(tmpZeroVec2, tmpZeroVec2, tmpZeroVec4, mTextureName);
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
		char currentChar = mTextContent.at(i);//(i < m_pTextContent->size()) ? m_pTextContent->at(i) : 0;


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


		NVECTOR2 realCharBitmapPixelSize = pFontMgr->IFactory<N_FontObject>::GetObjectPtr(mFontName)->mAsciiCharSizeList.at(currentChar);


#pragma region UpdatePositionOfSubRects

		//a base OFFSET to be added to vertices
		m_pGraphicObj->SetBasePosOffset(GetTopLeft());

		//calculate position of Rects (relative to the graphic object container)
		posAlignmentOffset.x = 0;
		posAlignmentOffset.y = (float)gFunc_GetCharAlignmentOffsetPixelY(mCharBoundarySizeY, UINT(realCharBitmapPixelSize.y), currentChar);

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
				IBasicContainerInfo::GetBasicColor(),
				mTextureName
				);
		}
		else
		{
			//common set
			m_pGraphicObj->SetRectangle(
				i,
				tmpRectTopLeft,
				tmpRectBottomRight,
				IBasicContainerInfo::GetBasicColor(),
				mTextureName
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


	mIsSizeChanged = false;
	mIsTextContentChanged = false;
//all chars update finished.
};

