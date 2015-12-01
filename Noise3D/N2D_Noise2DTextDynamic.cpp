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
	m_pTextColor = new NVECTOR4(0, 0, 0, 1.0f);
	m_pTextGlowColor = new NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pGraphicObj = new NoiseGraphicObject;
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
		auto& fontObj =m_pFatherFontMgr->m_pFontObjectList->at(fontID);

		//boundary size of 1 Char
		mCharBoundarySizeY = UINT(fontObj.mFontSize);
		mCharBoundarySizeX = mCharBoundarySizeY*UINT(fontObj.mAspectRatio);

		//update Texture ID
		std::stringstream tmpS;
		tmpS << "AsciiBitmapTable" << fontID;
		mStringTextureID = m_pFatherFontMgr->m_pTexMgr->GetTextureID(tmpS.str());
	}
};


void Noise2DTextDynamic::SetTextAscii(std::string newText)
{
	*m_pTextContent = newText;
};

void Noise2DTextDynamic::SetTextColor(NVECTOR4 color)
{
	*m_pTextColor = color;
}

void Noise2DTextDynamic::SetGlowColor(NVECTOR4 color)
{
	*m_pTextGlowColor = color;
}

void Noise2DTextDynamic::SetLineSpacingOffset(int offset)
{
	if (offset > -int(mCharBoundarySizeY))mLineSpacingOffset = offset;
};

void Noise2DTextDynamic::SetWordSpacingOffset(int offset)
{
	if (offset > -int(mCharBoundarySizeX))mWordSpacingOffset = offset;
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


	//2. update (set) rectangles to its new condition, and we store 2 string to avoid unnecessary 
	//texcoord update (char bitmap udpate)
	//like a cursor moving around
	NVECTOR2	posGeneralOffset(0, 0);
	NVECTOR2	posAlignmentOffset(0, 0);//some special characters need special alignment like 'g' 'p' 'q'
	for (UINT i = 0;i < stringCharCount;i++)
	{

		//chars which are at the corresponding position (if one string is longer than the other
		char currentChar = (i < m_pTextContent->size()) ? m_pTextContent->at(i) : 0;


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
		//calculate position of Rects (relative to the graphic object container)
		posAlignmentOffset.x = 0;
		posAlignmentOffset.y = (float)gFunction_GetCharAlignmentOffsetPixelY(mCharBoundarySizeY, UINT(realCharBitmapPixelSize.y), currentChar);

		NVECTOR2 tmpRectTopLeft(0, 0);
		NVECTOR2 tmpRectBottomRight(0, 0);
		tmpRectTopLeft = Noise2DBasicContainerInfo::GetTopLeft() + posGeneralOffset + posAlignmentOffset;
		tmpRectBottomRight = tmpRectTopLeft + NVECTOR2(float(mCharBoundarySizeX), float(mCharBoundarySizeY));
		
		//.......
		m_pGraphicObj->SetRectangle(
			i,
			tmpRectTopLeft,
			tmpRectBottomRight,
			Noise2DBasicContainerInfo::GetBasicColor(),
			mStringTextureID
			);
	
#pragma endregion UpdatePositionOfSubRects


		//update internal position offset ( to move the texcoord cursor of "TopLeft")
		posGeneralOffset.x += (realCharBitmapPixelSize.x+mWordSpacingOffset);

 		if (posGeneralOffset.x >= stringBoundaryWidth)
		{
			//cursor move to next line
			posGeneralOffset.y +=( mCharBoundarySizeY+mLineSpacingOffset);
			posGeneralOffset.x = 0.0f;
		}

	}//1 char update finished


//all chars update finished.
};

