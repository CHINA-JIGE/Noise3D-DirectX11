
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
		typedef Noise3D::Vec3 Radiance;
		typedef Noise3D::Vec3 Irradiance;
		class IPathTracerSoftShader;

		struct N_TraceRayPayload
		{
			Radiance radiance;
			float travelledDistance;
		};

		class /*_declspec(dllexport)*/ PathTracer
		{
		public:

			//entry of rendering a single frame to render target
			//first rebuild BVH of the singleton of CollisionTestor
			//then dispatch render task to render a tile (might be multi-threaded one day)
			//implementation of soft shaders will be passed in, and called by this path tracer render pipeline
			void Render(Noise3D::SceneNode* pNode, IPathTracerSoftShader* pShaders);

			//pixel size of a render tile
			void SetRenderTileSize(uint32_t width, uint32_t height);

			//default render target will be created at initialization automatically
			Texture2D* GetRenderTarget();

			//you can also set your own render target, but it must have a doubled copy of data in SYSTEM MEMORY
			void SetRenderTarget(Texture2D* pRenderTarget);//must be doubled-data

			//bounces count of light (recursive ray gen)
			void SetBounces(uint32_t bounces);

			uint32_t GetBounces();

			//ray's max travel distance
			void SetRayMaxTravelDist(float dist);

			float GetRayMaxTravelDist();

			//could be called by soft shader(and could be called recursively)
			void TraceRay(N_Ray& ray, N_TraceRayPayload& payload);

			//poll
			bool IsRenderFinished();

		protected:

			struct N_RenderTileInfo
			{
				N_RenderTileInfo():topLeftX(0), topLeftY(0), width(1), height(1){}
				N_RenderTileInfo(uint32_t x, uint32_t y, uint32_t w, uint32_t h) :
					topLeftX(x), topLeftY(y), width(w), height(h) {}

				uint32_t topLeftX;
				uint32_t topLeftY;
				uint32_t width;
				uint32_t height;
			};

			//render an image tile (let's say, 16x16, could be parallelized using multi-thread)
			//task:fire and trace rays(could be multi-threaded), 
			void RenderTile(const N_RenderTileInfo& info);

		private:

			friend SceneManager;//for external init

			friend class IFactory<PathTracer>;

			PathTracer();

			~PathTracer();

		private:

			//extern init by SceneManager
			bool	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(uint32_t pixelWidth, uint32_t pixelHeight);

			Texture2D* m_pRenderTarget;//created by SceneManager

			IPathTracerSoftShader* m_pShader;//path tracer's soft shader

			Noise3D::CollisionTestor* m_pCT;//singleton of collision testor

			uint32_t mBounces;

			float mRayMaxTravelDist;

			uint32_t mTileWidth;

			uint32_t mTileHeight;

			bool mIsRenderedFinished;
		};
	}
}