
/***********************************************************************

						h£ºNoiseGUI Basic Container Info

************************************************************************/

#pragma once


//a base class that encapsules basic information of a container
public class _declspec(dllexport) NoiseGUIBasicContainerInfo
{
public:
	NoiseGUIBasicContainerInfo();

	void  SetDiagonal(float x_topLeft, float y_topLeft, float x_bottomRight, float y_bottomRight);

	void  SetDiagonal(NVECTOR2 v_topLeft, NVECTOR2 v_bottomRight);

	void	SetCenterPos(float x, float y);

	void	SetCenterPos(NVECTOR2 v);

	void	SetWidth(float w);

	void	SetHeight(float h);

	void	SetVisible(BOOL isVisible);

	void	SetDragable(BOOL isDragable);

private:
	NVECTOR2*	m_pPositionCenter;
	BOOL	mIsDragable;
	BOOL	mIsVisible;
	float		mWidth;
	float		mHeight;
};

