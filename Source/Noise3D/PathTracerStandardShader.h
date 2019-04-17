
/***********************************************************

			GI: path tracer's STANDARD soft shader

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class PathTracerStandardShader :
			public IPathTracerSoftShader
		{
		public:

			virtual void ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer& hitInfo, N_TraceRayPayload& in_out_payload) override;

			virtual void Miss(N_Ray ray, N_TraceRayPayload& in_out_payload) override;

		private:

		};
	}
}
