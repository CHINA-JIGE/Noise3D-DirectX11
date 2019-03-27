
/***********************************************************************

											 h：主头文件

************************************************************************/

#pragma once

#pragma warning (disable : 4005)//Macro Redefined:  WIN8 SDK & DXSDK
//#define NDEBUG //
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN // exclude rarely used info from WINDOWS.h

#include <D3D11.h>
#include <dinput.h>
#include <D3Dcompiler.h>
#include <D3D11SDKLayers.h>
#include <vector>
#include <queue>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <map>
#include <type_traits>
#include <random>
#include <memory>

//Third Party : Microsoft's Effects11/FX11
#include <Effects11\d3dx11effect.h>

//Third Party : FreeType for font&text
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_TYPES_H
#include FT_IMAGE_H
#include FT_RENDER_H

//Third Party : FbxSdk for model loader
#include <fbxsdk.h>

//Third Party : DirectX Tex
#include <DirectXTex\DirectXTex.h>

//Third Party : DirectX Math's wrapper class SimpleMath
//#include <DirectXMath\DirectXMath.h>
#include <DirectXMath\SimpleMath\SimpleMath.h>

//------------
#include "NoiseMacro.h"
#include "NoiseTypes.h"
#include "NoiseGlobal.h"
#include "IFactory.h"
#include "TreeDataStructureTemplate.h"
#include "_2DBasicContainerInfo.h"
#include "FileManager.h"
#include "_GeometryMeshGenerator.h"
#include "_BasicRenderSettings.h"
#include "_Collidable.h"
#include "RigidTransform.h"
#include "AffineTransform.h"
#include "SceneGraph.h"
#include "ISceneObject.h"
#include "GeometryEntity.h"//geometry data container(RAM & VRAM)
#include "BvhTree.h"

#include "LogicalShape.h"
#include "LogicalSphere.h"
#include "LogicalBox.h"
#include "LogicalShapeManager.h"

#include "ModelProcessor.h"
#include "Camera.h"
#include "Atmosphere.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "ShadowCommonInterface.h"
#include "Lights.h"
#include "LightManager.h"
#include "ITexture.h"
#include "Texture2D.h"
#include "TextureCubeMap.h"
#include "TextureManager.h"
#include "Material.h"
#include "MaterialManager.h"
#include "GraphicObject.h"
#include "GraphicObjManager.h"
#include "SweepingTrail.h"
#include "SweepingTrailManager.h"
#include "CollisionTestor.h"
#include "MeshLoader.h"
#include "ShaderVarManager.h"

//---------Font & Text-----------
#include "Text_2DBasicTextInfo.h"
#include "Text_TextDynamic.h"
#include "Text_TextStatic.h"
#include "Text_TextManager.h"

//-------------Core Class-------------
#include "Renderer.h"
#include "Root.h"
#include "SceneManager.h"

//--------GI: Spherical Harmonic----------
#include "RandomSampleGenerator.h"
#include "ISphericalFunc.h"
#include "SHCommon.h"
#include "SHRotation.h"
#include "SHVector.h"

//-----------Noise Ut -----------
#include "Ut_Timer.h"
#include "Ut_MeshSlicer.h"
#include "Ut_InputEngine.h"
#include "Ut_Voxelizer.h"
#include "Ut_MCMeshReconstructor.h"


//-----------Noise GUI-------------
//#include "NoiseGUISystem.h"

;