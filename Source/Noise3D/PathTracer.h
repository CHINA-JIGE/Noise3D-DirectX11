
/***********************************************************

								GI: Path Tracer
		path-tracing based soft-renderer, for study purpose.
		the interface will be a little similar to DirectX Raytracing(DXR)

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class /*_declspec(dllexport)*/ PathTracer
		{
		public:

			void Render();

			ITexture* GetRenderTarget();

			void SetRenderTarget(ITexture* pRenderTarget);//must be doubled-data

		private:

			friend SceneManager;//for external init

			friend class IFactory<PathTracer>;

			PathTracer();

			~PathTracer();

		private:

			//extern init by SceneManager
			bool	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(uint32_t pixelWidth, uint32_t pixelHeight, ITexture* m_pRenderTarget);

			ITexture* m_pRenderTarget;//created by SceneManager

		};
	}
}