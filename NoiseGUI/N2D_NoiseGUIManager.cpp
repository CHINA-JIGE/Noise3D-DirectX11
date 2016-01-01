
/***********************************************************************

								¿‡£∫NOISE GUI manager

				Description : manage different GUI objects

************************************************************************/

#include "NoiseGUISystem.h"

static UINT	static_mouseScrPosX=0;
static UINT	static_mouseScrPosY = 0;
static float	static_windowSizeScaleX = 1.0f;
static float	static_windowSizeScaleY = 1.0f;
static float	static_mouseX_BackBufferSpace = 0.0;
static float	static_mouseY_BackBufferSpace = 0.0f;
static UINT	static_mousePosZ = 1;
static N_GUI_MOUSE_INFO static_mouseInfo = {};


NoiseGUIManager::NoiseGUIManager()
{
	m_pInputEngine = nullptr;
	m_pFontMgr = nullptr;
	m_pRenderer = nullptr;
	mCurrentTimeStamp = 0;
	m_pChildButtonList		= new std::vector<NoiseGUIButton*>;
	m_pChildTextList			= new std::vector<NoiseGUITextBox*>;
	m_pChildScrollBarList	= new std::vector<NoiseGUIScrollBar*>;
}

NoiseGUIManager::~NoiseGUIManager()
{
	delete m_pChildButtonList;
	delete m_pChildTextList;
	delete m_pChildScrollBarList;
}

BOOL NoiseGUIManager::Initialize(UINT backBufferWidth, UINT backBufferHeight, NoiseRenderer& refRenderer, NoiseUtInputEngine& refInputE, NoiseFontManager& refFontMgr, HWND hwnd)
{
	if (IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Initialized!");
		return FALSE;
	}

	m_pInputEngine = &refInputE;
	m_pFontMgr = &refFontMgr;
	m_pRenderer = &refRenderer;
	mBackBufferWidth = backBufferWidth>0?backBufferWidth:1;
	mBackBufferHeight = backBufferHeight>0 ? backBufferHeight : 1;

	//hwnd is needed for some win API invokation
	if (hwnd)
	{
		mWindowHWND = hwnd;
	}
	else
	{
		DEBUG_MSG1("Create GUI : hwnd Invaid!!");
		return FALSE;
	}

	SetStatusToBeInitialized();
	return TRUE;
}

BOOL NoiseGUIManager::InitButton(NoiseGUIButton& refObject)
{
	if (refObject.IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Initialized!");
		return FALSE;
	}

	//add button to child button list
	m_pChildButtonList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	//create graphic object
	 refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0, 0), NVECTOR2(100.0f, 50.0f),
		NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), NOISE_MACRO_INVALID_TEXTURE_ID);

	refObject.SetStatusToBeInitialized();

	//init Z pos/depth
	++mCurrentTimeStamp;
	refObject.SetDepthLevel(mCurrentTimeStamp);
	return TRUE;
}

BOOL NoiseGUIManager::InitTextBox(NoiseGUITextBox& refObject, UINT fontID)
{
	if (refObject.IsInitialized())
	{
		DEBUG_MSG1("TextBox Has Been Initialized!");
		return FALSE;
	}

	if (m_pFontMgr == nullptr)
	{
		DEBUG_MSG1("Font Manager Hasn't been set!");
		return FALSE;
	}

	//backGround
	refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0, 0), NVECTOR2(100.0f, 50.0f), NVECTOR4(0.3f, 0.3f, 1.0f, 1.0f), NOISE_MACRO_INVALID_TEXTURE_ID);
	//cursor
	refObject.m_pGraphicObj->AddRectangle(NVECTOR2(0, 0), NVECTOR2(20.0f, 3.0f), NVECTOR4(0.3f, 1.0f, 0.3f, 1.0f), NOISE_MACRO_INVALID_TEXTURE_ID);

	m_pFontMgr->InitDynamicTextA(fontID, "", 100, 50, NVECTOR4(0.3f, 1.0f, 0.3f, 1.0f), 3, 0, *refObject.m_pTextDynamic);

	//add label ( pure text)
	m_pChildTextList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	refObject.SetStatusToBeInitialized();

	//init Z pos/depth
	++mCurrentTimeStamp;
	refObject.SetDepthLevel(mCurrentTimeStamp);
	return TRUE;
}

BOOL NoiseGUIManager::InitScrollBar(NoiseGUIScrollBar & refObject)
{
	if (refObject.IsInitialized())
	{
		DEBUG_MSG1("Object Has Been Created!");
		return FALSE;
	}

	//initialized internal GUI button
	InitButton(*refObject.m_pButtonScrolling);
	refObject.m_pButtonScrolling->SetDiagonal(refObject.GetTopLeft(), refObject.GetTopLeft() + NVECTOR2(refObject.mScrollButtonLength, refObject.GetHeight()));
	refObject.m_pGraphicObj_Groove->AddRectangle(NVECTOR2(0,0),NVECTOR2(0.0f,0.0f),NVECTOR4(1.0f,1.0f,1.0f,1.0f));

	m_pChildScrollBarList->push_back(&refObject);
	refObject.m_pFatherGUIMgr = this;

	refObject.SetStatusToBeInitialized();

	//init Z pos/depth
	++mCurrentTimeStamp;
	refObject.SetDepthLevel(mCurrentTimeStamp);
	refObject.m_pButtonScrolling->SetDepthLevel(mCurrentTimeStamp);
	return TRUE;
}

void	NoiseGUIManager::SetInputEngine(NoiseUtInputEngine& refInputE)
{
	m_pInputEngine = &refInputE;
};

void NoiseGUIManager::SetFontManager(NoiseFontManager & refFontMgr)
{
	m_pFontMgr = &refFontMgr;
}

void NoiseGUIManager::SetRenderer(NoiseRenderer & refRenderer)
{
	m_pRenderer = &refRenderer;
};


void	NoiseGUIManager::Update()
{
	if (!IsInitialized())return;

	//input Engine must be set
	if (!m_pInputEngine)return;

	//if internal input engine was inoperative, then nth will be retrieved
	if (m_pInputEngine->IsInitialized())
	{
		//Get info from Input Engine (update STATIC variable)
		mFunction_UpdateInputEngineInfo();

		//..mouse click 'depth' (this can implement component overlapping)
		mFunction_ComputeMouseDepth();

		//traverse various kinds of objects and send message
		mFunction_UpdateButtons();

		mFunction_UpdateScrollBars();

		if (m_pFontMgr != nullptr)mFunction_UpdateTextBoxs();

		mFunction_UpdateAllInternalGraphicObjs();
	}
	else
	{
		DEBUG_MSG1("GUI: Input Engine is not initialized.");
	}

}

void NoiseGUIManager::SetWindowHWND(HWND hWnd)
{
	mWindowHWND = hWnd;
};


inline void NoiseGUIManager::AddObjectToRenderList(NoiseGUIButton & obj)
{
	//renderer must be set,then add object to GUI render List of renderer 
	m_pRenderer->AddObjectToRenderList(*obj.m_pGraphicObj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_GUI_OBJECT);
};

inline void NoiseGUIManager::AddObjectToRenderList(NoiseGUIScrollBar & obj)
{
	m_pRenderer->AddObjectToRenderList(*obj.m_pGraphicObj_Groove, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_GUI_OBJECT);
	m_pRenderer->AddObjectToRenderList(*obj.m_pButtonScrolling->m_pGraphicObj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_GUI_OBJECT);
};

inline void NoiseGUIManager::AddObjectToRenderList(NoiseGUITextBox & obj)
{
	m_pRenderer->AddObjectToRenderList(*obj.m_pTextDynamic, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_GUI_OBJECT);
	m_pRenderer->AddObjectToRenderList(*obj.m_pGraphicObj, NOISE_RENDERER_ADDTOLIST_OBJ_TYPE_GUI_OBJECT);
}


/***********************************************************************
										P R I V A T E
************************************************************************/
void NoiseGUIManager::Destroy()
{
	for (auto obj : *m_pChildButtonList)
	{
		if (obj)obj->SelfDestruction();
	}

	for (auto obj : *m_pChildTextList)
	{
		if (obj)obj->SelfDestruction();
	}

	for (auto obj : *m_pChildScrollBarList)
	{
		if (obj)obj->SelfDestruction();
	}
}

void NoiseGUIManager::mFunction_UpdateInputEngineInfo()
{
	//!!!!!!!!!!!!DONT FORGET THIS
	m_pInputEngine->Update();


	static_mouseScrPosX = m_pInputEngine->GetMouseScrPosX();
	static_mouseScrPosY = m_pInputEngine->GetMouseScrPosY();

	//get the Rect info of render window
	RECT windowRect;
	NOISE_MACRO_FUNCTION_WINAPI GetWindowRect(mWindowHWND, &windowRect);

	//we should check window style in case it's a window with FRAMES or CAPTION
	long windowLONG = NOISE_MACRO_FUNCTION_WINAPI GetWindowLongW(mWindowHWND, GWL_STYLE);
	UINT windowFrameWidth = NOISE_MACRO_FUNCTION_WINAPI  GetSystemMetrics(SM_CXFRAME);
	UINT windowFrameHeight = NOISE_MACRO_FUNCTION_WINAPI  GetSystemMetrics(SM_CYFRAME);
	UINT windowCaptionHeight = NOISE_MACRO_FUNCTION_WINAPI  GetSystemMetrics(SM_CYCAPTION);

	//this is a mode without window Caption
	if (windowLONG & WS_POPUP)
	{
		windowFrameWidth = 0;
		windowFrameHeight = 0;
		windowCaptionHeight = 0;
	}

	//remember , render window might enlarge or shrink , (didnt match the back buffer)
	//so mouse coord must be scaled to fit 
	//also note that actual render area don't include the window FRAMES & CAPTION
	static_windowSizeScaleX = (float)(windowRect.right - windowRect.left - windowFrameWidth * 2) / mBackBufferWidth;
	static_windowSizeScaleY = (float)(windowRect.bottom - windowRect.top - windowFrameHeight * 2 - windowCaptionHeight) / mBackBufferHeight;

	//relative cursor Pos (window scaling is taken into account)
	static_mouseX_BackBufferSpace = (static_mouseScrPosX - windowRect.left - windowFrameWidth) / static_windowSizeScaleX;
	static_mouseY_BackBufferSpace = (static_mouseScrPosY - windowRect.top - windowFrameHeight - windowCaptionHeight) / static_windowSizeScaleY;

	//mouseInfo
	static_mouseInfo.isMouseLeftPressedDown = m_pInputEngine->IsMouseButtonPressed(NOISE_MOUSEBUTTON_LEFT);
	static_mouseInfo.isMouseMoving = (m_pInputEngine->GetMouseDiffX() != 0) || (m_pInputEngine->GetMouseDiffY() != 0);
	static_mouseInfo.mouseX_backBufferSpace = static_mouseX_BackBufferSpace;
	static_mouseInfo.mouseY_backBufferSpace = static_mouseY_BackBufferSpace;
	static_mouseInfo.mouseDepthLevel = static_mousePosZ;
	static_mouseInfo.mouseScrollDiff = -float(m_pInputEngine->GetMouseScrollDiff()) / 30.0f;
}

void NoiseGUIManager::mFunction_ComputeMouseDepth()
{
	//each component has their own 'depth', also known as Z coordinate
	//the bigger the Z coord is , the deeper the component is put (more possible to be occluded)

	//so let's assume that the cursor lies within the area of serveral components,
	//which only the one with the smallest Z coord can be clicked.
	UINT currentMinDepth=NOISE_GUI_CONST_MAX_POSITION_Z;

	NVECTOR2 mousePos(static_mouseX_BackBufferSpace, static_mouseY_BackBufferSpace);

	for (auto obj : *m_pChildButtonList)
	{
		if (obj->IsInitialized() == FALSE)continue;
		//cursor within container && object has a smaller Z coord
		if (obj->IsPointInContainer(mousePos) == TRUE && currentMinDepth>obj->GetDepthLevel())currentMinDepth = obj->GetDepthLevel();
	}

	for (auto obj : *m_pChildScrollBarList)
	{
		if (obj->IsInitialized() == FALSE)continue;
		if (obj->IsPointInContainer(mousePos) == TRUE && currentMinDepth>obj->GetDepthLevel())currentMinDepth = obj->GetDepthLevel();
	}

	for (auto obj : *m_pChildTextList)
	{
		if (obj->IsInitialized() == FALSE)continue;
		if (obj->IsPointInContainer(mousePos) == TRUE && currentMinDepth>obj->GetDepthLevel())currentMinDepth = obj->GetDepthLevel();
	}

	//..find the smallest Z
	static_mousePosZ = currentMinDepth;

}

void NoiseGUIManager::mFunction_UpdateButtons()
{
	//!!!!!!!watch out for the multi-level of 'if'  embbeding
	for (auto pCurrButton : *m_pChildButtonList)
	{
		pCurrButton->mFunction_Update(
			pCurrButton->IsPointInContainer(NVECTOR2(static_mouseX_BackBufferSpace, static_mouseY_BackBufferSpace)),
			static_mouseInfo
			);
	}
}

void NoiseGUIManager::mFunction_UpdateScrollBars()
{

	//internal button has been updated,so DRAG operation has been done
	for (auto pBar : *m_pChildScrollBarList)
	{
		pBar->mFunction_Update(
			pBar->IsPointInContainer(NVECTOR2(static_mouseX_BackBufferSpace, static_mouseY_BackBufferSpace)),
			static_mouseInfo
			);
	}//next Scroll Bar

}

void NoiseGUIManager::mFunction_UpdateTextBoxs()
{
	BOOL isShiftPressed = (m_pInputEngine->IsKeyPressed(NOISE_KEY_LSHIFT)==TRUE)|| (m_pInputEngine->IsKeyPressed(NOISE_KEY_RSHIFT)==TRUE);
	BOOL isCapsLocked = m_pInputEngine->IsKeyPressed(NOISE_KEY_CAPITAL);

	const UINT maxScanCodeCount = 0xED;

	//get a list of 'whether the key is pressed'
	std::vector<BOOL> scanCodeStatusList;
	for (UINT scanCode = 0;scanCode < maxScanCodeCount;++scanCode)
	{
		scanCodeStatusList.push_back(m_pInputEngine->IsKeyPressed(scanCode));
	}

	//------------------------------------------------------------------------
	for (auto pTextBox : *m_pChildTextList)
	{
		pTextBox->mFunction_Update(
			pTextBox->IsPointInContainer(NVECTOR2(static_mouseX_BackBufferSpace, static_mouseY_BackBufferSpace)),
			static_mouseInfo,
			isShiftPressed,
			isCapsLocked,
			scanCodeStatusList
			);
	}//1 textbox
};

void NoiseGUIManager::mFunction_UpdateAllInternalGraphicObjs()
{
	//only update graphic objects, other internal component (e.g. button inside scrollBar)
	//will not be directly updated here...the reason that updating graphic objs after updating 
	//the infos of components is to wait for all container info are finally determined.NoiseGraphicObject is 
	//the real ,basic, graphic container

	//update buttons
	for (auto pCurrButton : *m_pChildButtonList)
	{
		if (pCurrButton->IsEnabled() == FALSE)continue;
		pCurrButton->mFunction_UpdateGraphicObject();
	}

	for (auto pBar : *m_pChildScrollBarList)
	{
		if (pBar->IsEnabled() == FALSE)continue;
		//only 1 rect for the groove texture
		pBar->mFunction_UpdateGraphicObject();
	}


	for (auto pTextBox : *m_pChildTextList)
	{
			if (pTextBox->IsEnabled() == FALSE)continue;
			pTextBox->mFunction_UpdateGraphicObject();
	}

}
