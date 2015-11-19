
/***********************************************************************

							class£ºNOISE GUI basic contain Info

					Description : encapsule basic container information like
					position, size ..... this is a base class to be inherited.

************************************************************************/

#include "Noise3D.h"

Noise2DBasicContainerInfo::Noise2DBasicContainerInfo()
{
	m_pPositionCenter = new NVECTOR2(50,30);
	mIsVisible = TRUE;
	mWidth = 100;
	mHeight = 60;
	m_pBasicColor = new NVECTOR4(0.3f,0.3f,1.0f,1.0f);
}

void Noise2DBasicContainerInfo::SetDiagonal(float x_topLeft, float y_topLeft,float x_bottomRight,float y_bottomRight)
{
	//should I check the sign of (posX-x) and (posY - y) ????
	if (x_bottomRight > x_topLeft && y_bottomRight > y_topLeft)
	{
		mWidth = x_bottomRight - x_topLeft;
		mHeight = y_bottomRight - y_topLeft;
		m_pPositionCenter->x = (x_bottomRight + x_topLeft) / 2.0f;
		m_pPositionCenter->y = (y_bottomRight + y_topLeft) / 2.0f;
	}
};

void Noise2DBasicContainerInfo::SetDiagonal(NVECTOR2 v_topLeft, NVECTOR2 v_bottomRight)
{
	//use another overloaded function
	SetDiagonal(v_topLeft.x, v_topLeft.y, v_bottomRight.x, v_bottomRight.y);
}

NVECTOR2 Noise2DBasicContainerInfo::GetTopLeft()
{
	return (*m_pPositionCenter)-NVECTOR2(mWidth/2,mHeight/2);
}

NVECTOR2 Noise2DBasicContainerInfo::GetBottomRight()
{
	return  (*m_pPositionCenter) + NVECTOR2(mWidth / 2, mHeight / 2);
};

void Noise2DBasicContainerInfo::SetCenterPos(float x, float y)
{
	m_pPositionCenter->x = x;
	m_pPositionCenter->y = y;
};

void Noise2DBasicContainerInfo::SetCenterPos(NVECTOR2 v)
{
	*(m_pPositionCenter) = v;
}

void Noise2DBasicContainerInfo::Move(float relativeX, float relativeY)
{
	SetCenterPos(m_pPositionCenter->x + relativeX, m_pPositionCenter->y + relativeY);
}

void Noise2DBasicContainerInfo::Move(NVECTOR2 relativePos)
{
	Move(relativePos.x, relativePos.y);
}

NVECTOR2 Noise2DBasicContainerInfo::GetCenterPos()
{
	return *m_pPositionCenter;
}

void Noise2DBasicContainerInfo::SetWidth(float w)
{
	//well , a button can't be that big
	if (w > 1 && w < 4096) mWidth = w;
};

void Noise2DBasicContainerInfo::SetHeight(float h)
{
	//well , a button can't be that big
	if (h > 1 && h < 4096)	mHeight = h;
}

float Noise2DBasicContainerInfo::GetWidth()
{
	return mWidth;
}

float Noise2DBasicContainerInfo::GetHeight()
{
	return mHeight;
}

void Noise2DBasicContainerInfo::SetVisible(BOOL isVisible)
{
	mIsVisible = isVisible;
}

BOOL Noise2DBasicContainerInfo::IsPointInContainer(NVECTOR2 v)
{
	return gFunction_IsPointInRect2D(
		v,
		*m_pPositionCenter - NVECTOR2(mWidth / 2, mHeight / 2),
		*m_pPositionCenter + NVECTOR2(mWidth / 2, mHeight / 2)
		);
};

void Noise2DBasicContainerInfo::SetBasicColor(NVECTOR4 c)
{
	*m_pBasicColor = c;
}

NVECTOR4 Noise2DBasicContainerInfo::GetBasicColor()
{
	return *m_pBasicColor;
};



