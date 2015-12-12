
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
#include <stdexcept>
#include <sstream>
#include <fstream>

//FreeType
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_TYPES_H
#include FT_IMAGE_H
#include FT_RENDER_H


#include "NoiseMacro.h"
#include "NoiseFlag.h"
#include "NoiseTypes.h"
#include "NoiseGlobal.h"

#include "NBASE_NoiseClassLifeCycle.h"
#include "NBASE_NoiseGUIEventCommonOperation.h"
#include "NBASE_Noise2DBasicContainerInfo.h"
#include "NBASE_NoiseFileManager.h"
#include "NBASE_Noise2DBasicTextInfo.h"
#include "N3D_NoiseEngine.h"
#include "N3D_NoiseMesh.h"
#include "N3D_NoiseCamera.h"
#include "N3D_NoiseLightManager.h"
#include "N3D_NoiseTextureManager.h"
#include "N3D_NoiseMaterialManager.h"
#include "N3D_NoiseAtmosphere.h"
#include "N2D_NoiseGraphicObject.h"
//---------Font & Text-----------
#include "N2D_Noise2DTextDynamic.h"
#include "N2D_Noise2DTextStatic.h"
#include "N2D_NoiseFontManager.h"
//--------------GUI-----------
#include "N2D_NoiseGUIButton.h"
#include "N2D_NoiseGUITextBox.h"
#include "N2D_NoiseGUIScrollBar.h"
#include "N2D_NoiseGUIManager.h"
//-------------Core Class-------------
#include "N3D_NoiseRenderer.h"
#include "N3D_NoiseScene.h"

//-----------Noise Ut -----------
#include "NUT_NoiseUtTimer.h"
#include "NUT_NoiseUtSlicer.h"
#include "NUT_NoiseUtInputEngine.h"
;