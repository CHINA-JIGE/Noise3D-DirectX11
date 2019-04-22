/***********************************************************************

				a header to place volatile header 
				(which is in development, and modified frequently)
				when done, headers included here can be transfered
				to "Noise3D_StableCommonHeader.h"

************************************************************************/
#pragma once

#include "RandomSampleGenerator.h"
#include "ISphericalFunc.h"
#include "SHCommon.h"
#include "SHRotation.h"
#include "SHVector.h"

#include "PathTracer.h"
#include "BxdfUt.h"
#include "_PathTracerSoftShaderInterface.hpp"
#include "PathTracerStandardShader.h"
#include "PathTracerShader_Minimal.h"
#include "PathTracerShader_Reflection.h"
#include "PathTracerShader_Diffuse.h"
#include "PathTracerShader_Refraction.h"
#include "PathTracerShader_AreaLighting.h"