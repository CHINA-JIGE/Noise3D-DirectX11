
/***********************************************************

			GI: implementation of path tracer soft shader

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class PathTracerShader_Sky :
			public IPathTracerSoftShader
		{
		public:

			void SetSkyTexture(Texture2D* pTex);

			virtual void ClosestHit(int bounces, float travelledDistance,const N_Ray& ray, const N_RayHitInfoForPathTracer& hitInfo, N_TraceRayPayload& in_out_payload) override;

			virtual void Miss(N_Ray ray, N_TraceRayPayload& in_out_payload) override;

		private:

			Texture2D* m_pSkyDomeTexture;
		};
	}
}