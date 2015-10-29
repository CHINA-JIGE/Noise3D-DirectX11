
/***********************************************************************

								h:Noise GUI Button

************************************************************************/

#pragma once



//A button ; can be embbed into other GUI object
public class _declspec(dllexport) NoiseGUIButton :public  NoiseGUIBasicContainerInfo
{
public:
	friend class NoiseGUIManager;

	NoiseGUIButton(UINT(*pFunc)(UINT NoiseGUIEvent) = nullptr);

	void	SetEventProcessCallbackFunction(UINT(*pFunc)(UINT NoiseGUIEvent));

	void	SetDragableX(BOOL dragableX);

	void	SetDragableY(BOOL dragableY);                                                                                                                               

	void	SetTexture_MouseAway(UINT texID);

	void	SetTexture_MouseOn(UINT texID);

	void	SetTexture_MousePressedDown(UINT texID);


	//HEY!!!!TELL ME WHY THERE INHERITED FUNCTIONS WERE NOT VISIBLE in EXE project ???????
	//THIS IS FXXKING UGLY !!!!
	void SetWidth(float w) { NoiseGUIBasicContainerInfo::SetWidth(w); }

	void  SetDiagonal(float x_topLeft, float y_topLeft, float x_bottomRight, float y_bottomRight){NoiseGUIBasicContainerInfo::SetDiagonal(x_topLeft,y_topLeft,x_bottomRight,y_bottomRight);}
	
	void  SetDiagonal(NVECTOR2 v_topLeft, NVECTOR2 v_bottomRight){NoiseGUIBasicContainerInfo::SetDiagonal(v_topLeft, v_bottomRight);}

	NVECTOR2 GetTopLeft(){return NoiseGUIBasicContainerInfo::GetTopLeft();}

	NVECTOR2 GetBottomRight(){return NoiseGUIBasicContainerInfo::GetBottomRight();}

	void	SetCenterPos(float x, float y){NoiseGUIBasicContainerInfo::SetCenterPos(x, y);}

	void	SetCenterPos(NVECTOR2 v) { NoiseGUIBasicContainerInfo::SetCenterPos(v); }

	void	Move(float relativeX, float relativeY) { NoiseGUIBasicContainerInfo::Move(relativeX, relativeY);}

	void	Move(NVECTOR2 relativePos) {NoiseGUIBasicContainerInfo::Move(relativePos);}

	NVECTOR2 GetCenterPos() {return NoiseGUIBasicContainerInfo::GetCenterPos(); }

	void	SetHeight(float h) { NoiseGUIBasicContainerInfo::SetHeight(h); }

	float	GetWidth() {return NoiseGUIBasicContainerInfo::GetWidth(); }

	float GetHeight() {return NoiseGUIBasicContainerInfo::GetHeight(); }

	void	SetVisible(BOOL isVisible) { NoiseGUIBasicContainerInfo::SetVisible(isVisible); }

	void	SetBasicColor(NVECTOR4 c) { NoiseGUIBasicContainerInfo::SetBasicColor(c); }

	NVECTOR4 GetBasicColor() {return NoiseGUIBasicContainerInfo::GetBasicColor(); }


private:
	NoiseGUIManager*		m_pFatherGUIMgr;
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