
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

			virtual GI::Radiance ClosestHit(N_Ray ray, const N_RayHitInfo& hitInfo, N_TraceRayPayload& in_out_payload) override;

			virtual GI::Radiance Miss(N_Ray ray, N_TraceRayPayload& in_out_payload) override;

		};
	}
}