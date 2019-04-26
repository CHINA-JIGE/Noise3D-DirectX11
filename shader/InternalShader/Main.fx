/**********************************************************************

			File: Main.fx
			Author: Jige
	
			Main FX Compilation entry

**************************************************************************/
//#define NOISE_NEW_FEATURE_IN_DEV

//declaration of structures and render classes
#include "CommonDef.hlsli"
#ifndef NOISE_NEW_FEATURE_IN_DEV
#include "DrawMesh.hlsli"
#endif
#include "Draw2D.hlsli"
#include "DrawSky.hlsli"
#include "DrawSolid3D.hlsli"
#include "RayMeshIntersection.hlsli"
#include "Picking.hlsli"
#include "DrawSweepingTrail.hlsli"
#include "PostProcessing.hlsli"


//implementation
#ifndef NOISE_NEW_FEATURE_IN_DEV
#include "DrawMesh_Common.fx"
#include "DrawMesh_Gouraud.fx"
#include "DrawMesh_Phong.fx"
#endif
#include "Draw2D.fx"
#include "DrawSky.fx"
#include "DrawSolid3D.fx"
#include "RayMeshIntersection.fx"
#include "Picking.fx"
#include "DrawSweepingTrail.fx"
#include "PostProcess_SimpleEffects.fx"
#include "PostProcess_Qwerty.fx"


//#include "shader3D_DrawMesh.fx"
