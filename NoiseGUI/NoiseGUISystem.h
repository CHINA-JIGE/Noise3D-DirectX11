
/***********************************************************************

									h£ºNoiseGUI

************************************************************************/

#pragma once

#include <Noise3D.h>

const UINT NOISE_GUI_CONST_MAX_POSITION_Z = 1000;//for components overlap
const UINT NOISE_GUI_CONST_TEXTBOX_DEFAULT_MAX_WORD_COUNT = 2000;
const float NOISE_GUI_CONST_LEVEL_DEPTH_INTERVAL = 1.0f / float(NOISE_GUI_CONST_MAX_POSITION_Z);

enum NOISE_GUI_BUTTON_STATE
{
	NOISE_GUI_BUTTON_STATE_COMMON = 0,
	NOISE_GUI_BUTTON_STATE_MOUSEON = 1,
	NOISE_GUI_BUTTON_STATE_MOUSEBUTTONDOWN = 2,
};

enum NOISE_GUI_EVENTS
{
	NOISE_GUI_EVENTS_COMMON_MOUSEDOWN = 1,
	NOISE_GUI_EVENTS_COMMON_MOUSEON = 2,
	NOISE_GUI_EVENTS_COMMON_MOUSEUP = 3,
	NOISE_GUI_EVENTS_COMMON_MOUSEMOVE = 4,
	NOISE_GUI_EVENTS_COMMON_GETFOCUS = 5,
	NOISE_GUI_EVENTS_COMMON_LOSTFOCUS = 6,

	NOISE_GUI_EVENTS_SCROLLBAR_SCROLL = 7,

	NOISE_GUI_EVENTS_TEXTBOX_TEXTCHANGE = 8,
};


#include "NBASE_NoiseGUIComponentBase.h"
#include "N2D_NoiseGUIButton.h"
#include "N2D_NoiseGUITextBox.h"
#include "N2D_NoiseGUIScrollBar.h"
#include "N2D_NoiseGUIOscilloscope.h"//Ê¾²¨Æ÷
#include "N2D_NoiseGUIManager.h"

