
/***********************************************************************

						h£ºNoiseGUI Basic Container Info

************************************************************************/

#pragma once


//a base class that encapsulate basic information of a container 
public class _declspec(dllexport) Noise2DBasicContainerInfo
{
public:
	Noise2DBasicContainerInfo();

	void  SetDiagonal(float x_topLeft, float y_topLeft, float x_bottomRight, float y_bottomRight);

	void  SetDiagonal(NVECTOR2 v_topLeft, NVECTOR2 v_bottomRight);

	NVECTOR2 GetTopLeft();

	NVECTOR2 GetBottomRight();

	void	SetCenterPos(float x, float y);

	void	SetCenterPos(NVECTOR2 v);

	void	Move(float relativeX, float relativeY);

	void	Move(NVECTOR2 relativePos);

	NVECTOR2 GetCenterPos();

	void	SetWidth(float w);

	void	SetHeight(float h);

	float	GetWidth();

	float GetHeight();

	void	SetVisible(BOOL isVisible);

    void	SetBasicColor(NVECTOR4 c);//if texture is invalid,use basic color to draw solid rect

	NVECTOR4 GetBasicColor();//if texture is invalid,use basic color to draw solid rect

protected:
	BOOL IsPointInContainer(NVECTOR2 v);

	NVECTOR2*	m_pPositionCenter;
	BOOL	mIsVisible;
	float		mWidth;
	float		mHeight;
	NVECTOR4*	m_pBasicColor;
};

