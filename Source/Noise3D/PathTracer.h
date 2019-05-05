
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
		
		enum NOISE_PATH_TRACER_SKYLIGHT_TYPE
		{
			NONE,
			SKY_DOME,
			SKY_BOX,
			SPHERICAL_HARMONIC
		};

		//input param for TraceRay()
		struct N_TraceRayParam
		{
			N_TraceRayParam():
				bounces(0),
				travelledDistance(0.0f), 
				ray(N_Ray()),
				isInsideObject(false),
				isSHEnvLight(false),
				skyLightType(SKY_DOME),
				isIndirectLightOnly(false),
				isShadowRay(false), 
				shadowRayLightSourceId(-1){}

			int bounces;//recursion count
			float travelledDistance;
			N_Ray ray;//which ray is tracing
			bool isInsideObject;//for refraction/transmission
			bool isSHEnvLight;
			NOISE_PATH_TRACER_SKYLIGHT_TYPE skyLightType;//might be used by Miss shader.
			bool isShadowRay;//for local lighting/area lighting
			bool isIndirectLightOnly;//for only indirect
			int shadowRayLightSourceId;//for shadow ray
		};

		//output for TraceRay();
		struct N_TraceRayPayload
		{
			N_TraceRayPayload() :radiance(0, 0, 0){};
			Radiance radiance;
			//float travelledDistance;
			//int bounces;
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
			//Texture2d* must be doubled-data
			//and one more internal R32G32B32A32 HDR render target
			void SetRenderTarget(Texture2D* pRenderTarget);

			// ray's maximum scatter count
			void SetMaxBounces(uint32_t bounces);

			uint32_t GetMaxBounces();

			void SetAmbientRadiance(GI::Radiance r);//when TraceRay() reach max bounce, what color should it return.

			//BxDF's max sample count
			void SetMaxDiffuseSampleCount(uint32_t sampleCount);

			void SetMaxSpecularScatterSample(uint32_t sampleCount);

			uint32_t GetMaxDiffuseSampleCount();

			uint32_t GetMaxSpecularScatterSampleCount();

			//ray's max travel distance
			void SetRayMaxTravelDist(float dist);

			float GetRayMaxTravelDist();

			//could be called by soft shader(and could be called recursively)
			//bounces should be added manually
			void TraceRay(const N_TraceRayParam& param, N_TraceRayPayload& out_payload);

			//poll
			bool IsRenderFinished();

			//end all rendering immediately
			void TerminateRenderTask();

			//image's exposure factor for tone mapping
			void SetExposure(float e);

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

			//4 or 8 or xx workers thread run independently,
			//actively pop out task and run it, until no task left
			void _RenderTileWorkerThread(std::queue<N_RenderTileInfo>& renderTaskList);

			//render an image tile (let's say, 16x16, could be parallelized using multi-thread)
			//task:fire and trace rays(could be multi-threaded), 
			void _RenderTile(const N_RenderTileInfo& info);

		private:

			friend SceneManager;//for external init

			friend class IFactory<PathTracer>;

			PathTracer();

			~PathTracer();

		private:

			static const uint32_t c_parallelWorkerThreadCount = 8;

			//extern init by SceneManager
			bool	NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init(uint32_t pixelWidth, uint32_t pixelHeight);

			void mFunction_ComputeLightSourceList(std::vector<GI::IGiRenderable*>& outList);

			Color4f mFunction_ToneMapping(Color4f c);


			std::vector<Color4f> mHdrRenderTarget;//temporary internal HDR render target

			Texture2D* m_pFinalRenderTarget;//created by SceneManager

			IPathTracerSoftShader* m_pShader;//path tracer's soft shader

			//multi-thread
			std::thread mWorkerThreadArr[c_parallelWorkerThreadCount];
			std::mutex mRenderTaskMutex;

			Noise3D::CollisionTestor* m_pCT;//singleton of collision testor

			uint32_t mMaxBounces;//max count of ray's recursion

			uint32_t mMaxDiffuseSampleCount;//max count of ray generated to evaluate a diffuse point

			uint32_t mMaxSpecularScatterSampleCount;//max sample count of microfacet-based specular scatter

			float mRayMaxTravelDist;

			uint32_t mTileWidth;

			uint32_t mTileHeight;

			bool mIsRenderedFinished;

			//log exposure
			float mLogExposurePreAmp;

			//returned radiance when reach max bounces
			GI::Radiance mAmbientRadiance;
		};
	}
}