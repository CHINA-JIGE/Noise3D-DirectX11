
/***********************************************************************

						h£ºNoiseGUI Basic Container Info

************************************************************************/

#pragma once


//a base class that encapsulate basic information of a container 
public class _declspec(dllexport) NoiseGUIBasicContainerInfo
{
public:
	NoiseGUIBasicContainerInfo();

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

    void	SetBasicColor(NVECTOR4 c);

	NVECTOR4 GetBasicColor();

protected:
	BOOL IsPointInContainer(NVECTOR2 v);

	NVECTOR2*	m_pPositionCenter;
	BOOL	mIsVisible;
	float		mWidth;
	float		mHeight;
	NVECTOR4*	m_pBasicColor;
};

