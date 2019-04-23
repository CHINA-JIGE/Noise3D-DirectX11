
/***********************************************************

			GI: implementation of path tracer soft shader

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class PathTracerShader_MinimalDemo:
			public IPathTracerSoftShader
		{
		public:

			virtual void ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer& hitInfo, N_TraceRayPayload& in_out_payload) override;

			virtual void  Miss(const N_TraceRayParam & param, N_TraceRayPayload& in_out_payload) override;

		};
	}
}