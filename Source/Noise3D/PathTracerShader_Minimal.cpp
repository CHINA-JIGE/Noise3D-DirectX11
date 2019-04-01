/***********************************************************

					Path Tracer Shader: Minimal

			Noise3D::PathTracer's soft shader "hello world", 
			for demonstration and test usage

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;

GI::Radiance Noise3D::GI::PathTracerShader_Minimal::ClosestHit(N_Ray ray, const N_RayHitInfo & hitInfo, N_TraceRayPayload & in_out_payload)
{
	return GI::Radiance();
}

GI::Radiance Noise3D::GI::PathTracerShader_Minimal::Miss(N_Ray ray, N_TraceRayPayload & in_out_payload)
{
	return GI::Radiance();
}
