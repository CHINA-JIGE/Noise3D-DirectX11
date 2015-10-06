
/***********************************************************************

h：主头文件

************************************************************************/

#pragma once

#pragma warning (disable : 4005)//宏重定义 WIN8 SDK和DXSDK
#define DIRECTINPUT_VERSION 0x0800

//#include <Windows.h>
#include <d3d11.h>
//#include <D3DX11.h>
#include <DxErr.h>
#include <D3DX10math.h>
#include <dinput.h>
#include <vector>
#include <sstream>
#include <fstream>

#include "NoiseMacro.h"
#include "NoiseFlag.h"
#include "NoiseTypes.h"
#include "NoiseGlobal.h"

#include "NBASE_NoiseFileManager.h"
#include "NUT_NoiseUtTimer.h"
#include "NUT_NoiseUtSlicer.h"
#include "NUT_NoiseUtInputEngine.h"
//#include "NUT_NoiseUtInputEngine_KeyDef.h"