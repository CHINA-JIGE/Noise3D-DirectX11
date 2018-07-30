/**********************************************************************

			File: Main.fx
			Author: Jige
	
			Main FX Compilation entry

**************************************************************************/
//declaration of structures and render classes
#include "CommonDef.hlsli"
#include "DrawMesh.hlsli"
#include "Draw2D.hlsli"
#include "DrawSky.hlsli"
#include "DrawSolid3D.hlsli"
#include "Picking.hlsli"
#include "DrawSweepingTrail.hlsli"
#include "PostProcessing.hlsli"


//implementation
#include "DrawMesh_Common.fx"
#include "DrawMesh_Gouraud.fx"
#include "DrawMesh_Phong.fx"
#include "Draw2D.fx"
#include "DrawSky.fx"
#include "DrawSolid3D.fx"
#include "Picking.fx"
#include "DrawSweepingTrail.fx"
#include "PostProcess_SimpleEffects.fx"
#include "PostProcess_Qwerty.fx"


//#include "shader3D_DrawMesh.fx"
