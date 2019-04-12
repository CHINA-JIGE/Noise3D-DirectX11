
/***********************************************************

			GI: implementation of path tracer soft shader

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class PathTracerShader_Minimal:
			public IPathTracerSoftShader
		{
		public:

			virtual void ClosestHit(int diffuseBounces, int specularBounces, float travelledDistance, const N_Ray& ray, const N_RayHitInfoForPathTracer& hitInfo, N_TraceRayPayload& in_out_payload) override;

			virtual void  Miss(N_Ray ray, N_TraceRayPayload& in_out_payload) override;

		};
	}
}