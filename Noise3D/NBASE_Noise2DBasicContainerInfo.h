
/***********************************************************************

						h£ºNoiseGUI Basic Container Info


			Description : encapsule basic container information like
			position, size ..... this is a base class to be inherited.


************************************************************************/

#pragma once

//a base class that encapsulate basic information of a container 
class  _declspec(dllexport)  Noise2DBasicContainerInfo
{
public:
	Noise2DBasicContainerInfo();

	void  SetDiagonal(float x_topLeft, float y_topLeft, float x_bottomRight, float y_bottomRight);

	void  SetDiagonal(NVECTOR2 v_topLeft, NVECTOR2 v_bottomRight);

	NVECTOR2 GetTopLeft();

	NVECTOR2 GetBottomRight();

	void	SetCenterPos(float x, float y) ;

	void	SetCenterPos(NVECTOR2 v);

	void	Move(float relativeX, float relativeY);

	void	Move(NVECTOR2 relativePos);

	NVECTOR2 GetCenterPos();

	void	SetWidth(float w);

	void	SetHeight(float h);

	float	GetWidth();

	float GetHeight();

    void	SetBasicColor(NVECTOR4 c);//if texture is invalid,use basic color to draw solid rect

	NVECTOR4 GetBasicColor();//if texture is invalid,use basic color to draw solid rect

	void	SetEnabled(BOOL isEnabled);

	BOOL IsEnabled();

protected:
	BOOL	 IsPointInContainer(NVECTOR2 v);
	BOOL	mIsEnabled;
	NVECTOR2*	m_pPositionCenter;
	float		mWidth;
	float		mHeight;
	NVECTOR4*	m_pBasicColor;
};
