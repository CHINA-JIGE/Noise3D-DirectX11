
/***********************************************************************

                           h：主头文件

************************************************************************/

#pragma once

#pragma warning (disable : 4005)//宏重定义 WIN8 SDK和DXSDK
#define NDEBUG //禁止调用assert
#define DIRECTINPUT_VERSION 0x0800


#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <dinput.h>
#include <DxErr.h>
#include <D3DX10math.h>
#include <D3Dcompiler.h>
#include <D3D11SDKLayers.h>
#include <vector>
#include <sstream>
#include <fstream>

#include "NoiseMacro.h"
#include "NoiseFlag.h"
#include "NoiseTypes.h"
#include "NoiseGlobal.h"

#include "NBASE_NoiseFileManager.h"
#include "N3D_NoiseEngine.h"
#include "N3D_NoiseMesh.h"
#include "N3D_NoiseCamera.h"
#include "N3D_NoiseRenderer.h"
#include "N3D_NoiseLightManager.h"
#include "N3D_NoiseTextureManager.h"
#include "N3D_NoiseMaterialManager.h"
#include "N3D_NoiseAtmosphere.h"
#include "N2D_NoiseGUI.h"

#include "N2D_NoiseGraphicObject.h"
#include "N3D_NoiseScene.h"

#include "NUT_NoiseUtTimer.h"
#include "NUT_NoiseUtSlicer.h"
#include "NUT_NoiseUtInputEngine.h"
;