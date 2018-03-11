
/***********************************************************************

											 h：主头文件

************************************************************************/

#pragma once

#pragma warning (disable : 4005)//Macro Redefined:  WIN8 SDK & DXSDK
//#define NDEBUG //
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN // exclude rarely used info from WINDOWS.h

#include <D3D11.h>
#include <D3DX\D3DX11.h>//D3DXCompile
#include <D3DX\DxErr.h>
#include <D3DX\D3DX10math.h>
#include <d3dx11effect.h>//fx11
#include <dinput.h>
#include <D3Dcompiler.h>
#include <D3D11SDKLayers.h>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <map>

//FreeType for font&text
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_TYPES_H
#include FT_IMAGE_H
#include FT_RENDER_H

//FbxSdk for model loader
#include <fbxsdk.h>

//directXTex
#include <DirectXTex\DirectXTex.h>
//------------
#include "NoiseMacro.h"
#include "NoiseTypes.h"
#include "NoiseGlobal.h"
#include "IFactory.h"
#include "_2DBasicContainerInfo.h"
#include "_SpatialHashTable.h"
#include "FileManager.h"
#include "_GeometryMeshGenerator.h"
#include "_BasicRenderSettings.h"
#include "Mesh.h"
#include "ModelProcessor.h"
#include "Lights.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"
#include "Atmosphere.h"
#include "GraphicObject.h"
#include "MeshManager.h"
#include "LightManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "GraphicObjMgr.h"
#include "CollisionTestor.h"
#include "ModelLoader.h"
#include "ShaderVarManager.h"

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
#include "Ut_Voxelizer.h"
#include "Ut_MCMeshReconstructor.h"


//-----------Noise GUI-------------
//#include "NoiseGUISystem.h"

;