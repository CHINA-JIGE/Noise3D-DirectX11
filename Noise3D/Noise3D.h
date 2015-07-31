
/***********************************************************************

                           h：主头文件

************************************************************************/

#pragma once

#pragma warning (disable : 4005)//宏重定义 WIN8 SDK和DXSDK
#define NDEBUG //禁止调用assert
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>
#include <DxErr.h>
#include <D3DX10math.h>
#include <D3Dcompiler.h>
#include <D3D11SDKLayers.h>
#include <assert.h>
#include <vector>
#include <list>
#include <sstream>
#include <fstream>

#include "NoiseMacro.h"
#include "NoiseFlag.h"
#include "NoiseTypes.h"
#include "NoiseGlobal.h"

#include "N3D_NoiseFileManager.h"
#include "N3D_NoiseEngine.h"
#include "N3D_NoiseMesh.h"
#include "N3D_NoiseCamera.h"
#include "N3D_NoiseRenderer.h"
#include "N3D_NoiseLightManager.h"
#include "N3D_NoiseLineBuffer.h"
#include "N3D_NoiseScene.h"
#include "NUT_NoiseUtTimer.h"
#include "NUT_NoiseUtSlicer.h"


;