
/***********************************************************************

								h:Noise GUI Button

************************************************************************/

#pragma once

//A button ; can be embbed into other GUI object
class _declspec(dllexport) NoiseGUIButton :
	public	Noise2DBasicContainerInfo,
	public	NoiseClassLifeCycle,
	public	NoiseGUIEventCommonOperation
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;

	NoiseGUIButton(UINT(*pFunc)(UINT NoiseGUIEvent) = nullptr);

	void	SetDragableX(BOOL dragableX);

	void	SetDragableY(BOOL dragableY);                                                                                                                               

	void	SetTexture(NOISE_GUI_BUTTON_STATE btnState, UINT texID);

private:

	void	Destroy();

	NoiseGUIManager*		m_pFatherGUIMgr;
	NoiseGraphicObject*		m_pGraphicObj;

	NOISE_GUI_BUTTON_STATE mButtonState;
	BOOL	mButtonHasBeenPressedDown;
	NVECTOR2* m_pMouseDown_OffsetFromCenter;//used when dragable , to keep the relative position unchanged to cursor

	//textures for 3 different state
	UINT		mTextureID_MouseAway;
	UINT		mTextureID_MouseOn;
	UINT		mTextureID_MousePressedDown;

	BOOL	mIsDragableX;//decide if it can be dragged to move in X dir
	BOOL	mIsDragableY;//decide if it can be dragged to move in Y dir

};