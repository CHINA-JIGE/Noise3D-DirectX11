
/***********************************************************************

								h:Noise GUI Button

************************************************************************/

#pragma once

//A button ; can be embbed into other GUI object
public class _declspec(dllexport) NoiseGUIButton :public  Noise2DBasicContainerInfo//common 2D container op
{
public:
	friend class NoiseGUIManager;
	friend class NoiseRenderer;

	NoiseGUIButton(UINT(*pFunc)(UINT NoiseGUIEvent) = nullptr);

	void	SetEventProcessCallbackFunction(UINT(*pFunc)(UINT NoiseGUIEvent));

	void	SetDragableX(BOOL dragableX);

	void	SetDragableY(BOOL dragableY);                                                                                                                               

	void	SetTexture_MouseAway(UINT texID);

	void	SetTexture_MouseOn(UINT texID);

	void	SetTexture_MousePressedDown(UINT texID);

private:
	NoiseGUIManager*		m_pFatherGUIMgr;
	NoiseGraphicObject*		m_pGraphicObj;
	UINT  (*m_pFunction_EventMessageProcess)(UINT NoiseGUIEvent);//a callback function pointer
	UINT		mGraphicObject_RectID;//index of rectangle in main graphic object
	NOISE_GUI_BUTTON_STATE mButtonState;
	BOOL	mButtonHasBeenPressedDown;

	//textures for 3 different state
	UINT		mTextureID_MouseAway;
	UINT		mTextureID_MouseOn;
	UINT		mTextureID_MousePressedDown;

	BOOL	mIsDragableX;//decide if it can be dragged to move in X dir
	BOOL	mIsDragableY;//decide if it can be dragged to move in Y dir

};