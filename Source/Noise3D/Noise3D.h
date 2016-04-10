
/***********************************************************************

                           h：主头文件

************************************************************************/

#pragma once

#pragma warning (disable : 4005)//宏重定义 WIN8 SDK和DXSDK
#define NDEBUG //禁止调用assert
#define DIRECTINPUT_VERSION 0x0800

#include "d3d11.h"
#include "D3DX11.h"
#include "d3dx11effect.h"
#include "dinput.h"
#include "DxErr.h"
#include "D3DX10math.h"
#include "D3Dcompiler.h"
#include "D3D11SDKLayers.h"
#include <vector>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <functional>
#include <unordered_map>

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

#include "NBASE_IFactory.h"
#include "NBASE_2DBasicContainerInfo.h"
#include "NBASE_FileManager.h"
#include "NBASE_2DBasicTextInfo.h"
#include "NBASE_GeometryMeshGenerator.h"
#include "N3D_Root.h"
#include "N3D_Mesh.h"
#include "N3D_Camera.h"
#include "N3D_LightManager.h"
#include "N3D_TextureManager.h"
#include "N3D_MaterialManager.h"
#include "N3D_Atmosphere.h"
#include "N2D_GraphicObject.h"
//---------Font & Text-----------
#include "N2D_TextDynamic.h"
#include "N2D_TextStatic.h"
#include "N2D_FontManager.h"
//-------------Core Class-------------
#include "N3D_Renderer.h"
#include "N3D_Scene.h"

//-----------Noise Ut -----------
#include "NUT_NoiseUtTimer.h"
#include "NUT_NoiseUtSlicer.h"
#include "NUT_NoiseUtInputEngine.h"

//-----------Noise GUI-------------
#include "NoiseGUISystem.h"

;