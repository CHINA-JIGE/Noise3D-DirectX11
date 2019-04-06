/***********************************************************

									Path Tracer
			path-tracing based soft-renderer, for study purpose.
	the interface will be a little similar to DirectX Raytracing (DXR)

***********************************************************/
#include "Noise3D.h"

using namespace Noise3D;

Noise3D::GI::PathTracer::PathTracer():
	m_pRenderTarget(nullptr),
	m_pShader(nullptr),
	mBounces(0),
	mRayMaxTravelDist(100.0f),
	mTileWidth(16),
	mTileHeight(16),
	mIsRenderedFinished(false)
{
	m_pCT = Noise3D::GetScene()->GetCollisionTestor();
}

Noise3D::GI::PathTracer::~PathTracer()
{
	m_pRenderTarget = nullptr;
	m_pCT = nullptr;
	m_pShader = nullptr;
}

void Noise3D::GI::PathTracer::Render(Noise3D::SceneNode * pNode, IPathTracerSoftShader* pShader)
{
	if (pNode == nullptr || pShader==nullptr)
	{
		WARNING_MSG("Path Tracer: parameter ptr is nullptr. nothing will be rendered.");
		return;
	}

	//reset render task state
	mIsRenderedFinished = false;

	//re-build BVH to accelerate ray-object intersection
	m_pCT->RebuildBvhTree(pNode);

	//set soft shader
	m_pShader = pShader;
	pShader->_InitInfrastructure(this, m_pCT);

	//back buffer's size, partition it into tiles
	uint32_t w = m_pRenderTarget->GetWidth();
	uint32_t h = m_pRenderTarget->GetHeight();
	uint32_t completeTileCountX = (w / mTileWidth);
	uint32_t completeTileCountY = (h / mTileHeight);
	uint32_t partialTileWidth = w % mTileWidth;//some tile might be incomplete
	uint32_t partialTileHeight = h % mTileHeight;//some tile might be incomplete

	//generate render tile task info list
	std::vector<N_RenderTileInfo> renderTaskList;
	for (uint32_t tileIdY = 0; tileIdY <= completeTileCountY; ++tileIdY)
	{
		for (uint32_t tileIdX = 0; tileIdX <= completeTileCountX; ++tileIdX)
		{
			//incomplete block at the right/bottom edge are considered
			uint32_t currentTileW = (tileIdX != completeTileCountX ? mTileWidth : partialTileWidth);
			uint32_t currentTileH = (tileIdY != completeTileCountY ? mTileHeight : partialTileHeight);

			N_RenderTileInfo info;
			info.topLeftX = mTileWidth * tileIdX;
			info.topLeftY = mTileHeight * tileIdY;
			info.width = currentTileW;
			info.height = currentTileH;

			//ignore edge tile with no pixel
			if (currentTileW>0 && currentTileH >0)
			{
				renderTaskList.push_back(info);
				//PathTracer::RenderTile(info);
			}
		}
	}

	//dispatch render tasks, run several thread/async at the same time
	//calculate thread cluster count and the thread count of the last cluster
	const uint32_t c_parallelTaskCount = 8;
	uint32_t numTaskClusters = renderTaskList.size() / c_parallelTaskCount;
	uint32_t numLastClusterThreadCount = renderTaskList.size() % c_parallelTaskCount;
	if (numLastClusterThreadCount != 0)
		numTaskClusters++;
	else 
		numLastClusterThreadCount = c_parallelTaskCount;

	//dispatch (std::async & get) or (std::thread & join)
	//if system budget is tight, std::async might choose not to create a thread
	//or create a thread in a deferred way
	struct PathTracerRenderTileFunctor
	{
		void operator()(PathTracer* p, const N_RenderTileInfo& info){p->RenderTile(info);}
	};

	//dispatch all render tasks (c_parallelTaskCount * numTaskClusters + numLastClusterThreadCount)
	for(uint32_t i=0; i<numTaskClusters;++i)
	{
		uint32_t localTaskCount = (i == numTaskClusters - 1 ?
			numLastClusterThreadCount : c_parallelTaskCount);

		//dispatch async task (probably threads)
		std::future<void> returnedVal[c_parallelTaskCount];
		//std::thread threadGroup[c_parallelTaskCount];
		for (uint32_t localTaskId = 0; localTaskId < localTaskCount; ++localTaskId)
		{
			uint32_t globalTaskId = i * c_parallelTaskCount + localTaskId;
			PathTracerRenderTileFunctor renderTileFunctor;
			returnedVal[localTaskId] = std::async(renderTileFunctor, this, renderTaskList.at(globalTaskId));
			//threadGroup[localTaskId] = std::thread(renderTileFunctor, this, renderTaskList.at(globalTaskId));
		}

		//threads join here
		for (uint32_t j= 0; j< localTaskCount; ++j)
		{
			returnedVal[j].get();
			//threadGroup[localTaskId].join();
		}
	}

	mIsRenderedFinished = true;
}

void Noise3D::GI::PathTracer::SetRenderTileSize(uint32_t width, uint32_t height)
{
	mTileWidth = width > 0 ? width : 1;
	mTileHeight = height > 0 ? height : 1;
}

Texture2D * Noise3D::GI::PathTracer::GetRenderTarget()
{
	return m_pRenderTarget;
}

void Noise3D::GI::PathTracer::SetRenderTarget(Texture2D * pRenderTarget)
{
	if (pRenderTarget != nullptr)
	{
		if (pRenderTarget->IsSysMemBufferValid())
		{
			m_pRenderTarget = pRenderTarget;
		}
		else
		{
			WARNING_MSG("PathTracer: render target doesn't have doubled data in system memory.");
		}
	}
}

void Noise3D::GI::PathTracer::SetMaxBounces(uint32_t bounces)
{
	mBounces = bounces;
}

uint32_t Noise3D::GI::PathTracer::GetMaxBounces()
{
	return mBounces;
}

void Noise3D::GI::PathTracer::SetRayMaxTravelDist(float dist)
{
	mRayMaxTravelDist = dist;
}

float Noise3D::GI::PathTracer::GetRayMaxTravelDist()
{
	return mRayMaxTravelDist;
}

/***********************************************

							PRIVATE

***********************************************/
bool NOISE_MACRO_FUNCTION_EXTERN_CALL Noise3D::GI::PathTracer::mFunction_Init(uint32_t pixelWidth, uint32_t pixelHeight)
{
	//create back buffer for path tracer(must enable CPU doubled data)
	if (pixelWidth < 16)pixelWidth = 16;
	if (pixelHeight < 16)pixelHeight = 16;
	TextureManager* pTexMgr = Noise3D::GetScene()->GetTextureMgr();
	Texture2D* pRenderTarget = pTexMgr->CreatePureColorTexture(
		"default_pathTracerRT",
		pixelWidth, pixelHeight, Color4u(0, 0, 0, 0), true);

	if (pRenderTarget == nullptr)
	{
		ERROR_MSG("PathTracer: failed to create path tracer's default back buffer/texture2d.");
		return false;
	}

	m_pRenderTarget = pRenderTarget;
	return true;
}

void Noise3D::GI::PathTracer::RenderTile(const N_RenderTileInfo & info)
{
	Camera* pCam = Noise3D::GetScene()->GetCamera();
	for (uint32_t x = 0; x < info.width; ++x)
	{
		for (uint32_t y = 0; y < info.height; ++y)
		{
			N_Ray ray = pCam->FireRay_WorldSpace(
				PixelCoord2(float(info.topLeftX+x), float(info.topLeftY+y)), 
				m_pRenderTarget->GetWidth(), 
				m_pRenderTarget->GetHeight());

			//start tracing a ray with payload
			N_TraceRayPayload payload;
			TraceRay(0, 0.0f, ray, payload);

			//set one pixel at a time 
			//(it's ok, one pixel is not easy to evaluate, setpixel won't be a big overhead)
			Color4f tmpColor = Color4f(payload.radiance.x, payload.radiance.y, payload.radiance.z, 1.0f);
			Color4u outputColor(tmpColor);//convert to 8bitx4 color via constructor
			m_pRenderTarget->SetPixel(info.topLeftX + x, info.topLeftY + y, outputColor);
		}
	}
}

void Noise3D::GI::PathTracer::TraceRay(int bounces, float travelledDistance,const N_Ray & ray, N_TraceRayPayload & payload)
{
	//initial bounces and travelled distance must remain in limit
	if (bounces > PathTracer::GetMaxBounces() || 
		travelledDistance > PathTracer::GetRayMaxTravelDist())return;

	//intersect the ray with the scene and get results
	N_RayHitResultForPathTracer hitResult;
	m_pCT->IntersectRaySceneForPathTracer(ray, hitResult);

	//update bounce count
	int newBounces = bounces + 1;
	if (newBounces > PathTracer::GetMaxBounces())return;

	//call shader. Radiance and other infos are carried by Payload reference.
	if (hitResult.HasAnyHit())
	{
		int index = hitResult.GetClosestHitIndex();
		N_RayHitInfoForPathTracer& info = hitResult.hitList.at(index);

		//update ray's travelled distance
		float newTravelledDistance = travelledDistance + ray.Distance(info.t);
		if (newTravelledDistance > PathTracer::GetRayMaxTravelDist())return;

		m_pShader->ClosestHit(newBounces, newTravelledDistance, ray, info, payload);
	}
	else
	{
		m_pShader->Miss(ray, payload);
	}

}

bool Noise3D::GI::PathTracer::IsRenderFinished()
{
	return mIsRenderedFinished;
}
