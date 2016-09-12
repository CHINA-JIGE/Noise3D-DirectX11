
/***********************************************************************

                           h：主头文件

************************************************************************/

#pragma once

#pragma warning (disable : 4005)//宏重定义 WIN8 SDK和DXSDK
#define NDEBUG //禁止调用assert
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN // exclude rarely used info from WINDOWS.h

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

//------------
#include "NoiseMacro.h"
#include "NoiseFlag.h"
#include "NoiseTypes.h"
#include "NoiseGlobal.h"
#include "GetResource.h"

#include "IFactory.h"
#include "_2DBasicContainerInfo.h"
#include "FileManager.h"
#include "GeometryMeshGenerator.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Lights.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"
#include "MeshManager.h"
#include "LightManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "Atmosphere.h"
#include "GraphicObject.h"
#include "GraphicObjMgr.h"
//---------Font & Text-----------
#include "Text_2DBasicTextInfo.h"
#include "Text_TextDynamic.h"
#include "Text_TextStatic.h"
#include "Text_FontManager.h"
//-------------Core Class-------------
#include "Renderer.h"
#include "Root.h"
#include "Scene.h"

//-----------Noise Ut -----------
#include "Ut_Timer.h"
#include "Ut_MeshSlicer.h"
#include "Ut_InputEngine.h"


//-----------Noise GUI-------------
//#include "NoiseGUISystem.h"

;