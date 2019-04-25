/***********************************************************

									Path Tracer
			path-tracing based soft-renderer, for study purpose.
	the interface will be a little similar to DirectX Raytracing (DXR)

***********************************************************/
#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

Noise3D::GI::PathTracer::PathTracer():
	m_pFinalRenderTarget(nullptr),
	m_pShader(nullptr),
	mMaxBounces(2),
	mMaxDiffuseSampleCount(64),
	mMaxSpecularScatterSampleCount(64),
	mRayMaxTravelDist(100.0f),
	mTileWidth(16),
	mTileHeight(16),
	mIsRenderedFinished(false),
	mLogExposurePreAmp(1.0f)
{
	m_pCT = Noise3D::GetScene()->GetCollisionTestor();
}

Noise3D::GI::PathTracer::~PathTracer()
{
	PathTracer::TerminateRenderTask();
	m_pFinalRenderTarget = nullptr;
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

	//cache world transform(avoid redundant computation)
	std::vector<SceneNode*> nodeList;
	SceneGraph* pSG = pNode->GetHostTree();
	pSG->Traverse_PreOrder(pNode, nodeList);
	for (auto node : nodeList)node->EvalWorldTransform(true);

	//reset render task state
	mIsRenderedFinished = false;

	//re-build BVH to accelerate ray-object intersection
	m_pCT->RebuildBvhTreeForGI(pNode);

	//set soft shader and pass light source list
	m_pShader = pShader;
	std::vector<GI::IGiRenderable*> lightSourceList;
	mFunction_ComputeLightSourceList(lightSourceList);
	pShader->_InitInfrastructure(this, m_pCT, std::move(lightSourceList));

	//back buffer's size, partition it into tiles
	uint32_t w = m_pFinalRenderTarget->GetWidth();
	uint32_t h = m_pFinalRenderTarget->GetHeight();
	uint32_t completeTileCountX = (w / mTileWidth);
	uint32_t completeTileCountY = (h / mTileHeight);
	uint32_t partialTileWidth = w % mTileWidth;//some tile might be incomplete
	uint32_t partialTileHeight = h % mTileHeight;//some tile might be incomplete

	//generate render tile task info list
	std::queue<N_RenderTileInfo> renderTaskList;
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
				renderTaskList.push(info);
			}
		}
	}


	//dispatch several 'renderTile' worker thread (runs independently),
	//each of them pop one render task from front and do it
	//but remember to lock the renderTaskList before popping front
	std::mutex mutex_renderTaskList;

	//thread functor
	struct PathTracerRenderTileFunctor
	{
		void operator()(PathTracer* p, std::queue<N_RenderTileInfo>& renderTaskList)
		{
			p->_RenderTileWorkerThread(renderTaskList);
		}
	} RenderTileWorkerFunc;

	//dispatch worker threads
	for (uint32_t i = 0; i < c_parallelWorkerThreadCount; ++i)
	{
		mWorkerThreadArr[i] = std::thread(RenderTileWorkerFunc, this, std::ref(renderTaskList));
	}
	
	//synchronize (all jobs are done now)
	for (uint32_t i = 0; i < c_parallelWorkerThreadCount; ++i)
	{
		if(mWorkerThreadArr[i].joinable())mWorkerThreadArr[i].join();
	}

	//clear cached world transform computed at the beginning of path tracer render
	for (auto node : nodeList)node->ClearWorldTransformCache();
	mIsRenderedFinished = true;
}

void Noise3D::GI::PathTracer::SetRenderTileSize(uint32_t width, uint32_t height)
{
	mTileWidth = width > 0 ? width : 1;
	mTileHeight = height > 0 ? height : 1;
}

Texture2D * Noise3D::GI::PathTracer::GetRenderTarget()
{
	return m_pFinalRenderTarget;
}

void Noise3D::GI::PathTracer::SetRenderTarget(Texture2D * pRenderTarget)
{
	if (pRenderTarget != nullptr)
	{
		if (pRenderTarget->IsSysMemBufferValid())
		{
			m_pFinalRenderTarget = pRenderTarget;
			mHdrRenderTarget.resize(pRenderTarget->GetWidth()* pRenderTarget->GetHeight());
		}
		else
		{
			WARNING_MSG("PathTracer: render target doesn't have doubled data in system memory.");
		}
	}
}


void Noise3D::GI::PathTracer::SetMaxBounces(uint32_t bounces)
{
	mMaxBounces = bounces;
}

uint32_t Noise3D::GI::PathTracer::GetMaxBounces()
{
	return mMaxBounces;
}

void Noise3D::GI::PathTracer::SetMaxDiffuseSampleCount(uint32_t sampleCount)
{
	mMaxDiffuseSampleCount = sampleCount;
}

uint32_t Noise3D::GI::PathTracer::GetMaxDiffuseSampleCount()
{
	return mMaxDiffuseSampleCount;
}

void Noise3D::GI::PathTracer::SetMaxSpecularScatterSample(uint32_t sampleCount)
{
	mMaxSpecularScatterSampleCount = sampleCount;
}

uint32_t Noise3D::GI::PathTracer::GetMaxSpecularScatterSampleCount()
{
	return mMaxSpecularScatterSampleCount;
}

void Noise3D::GI::PathTracer::SetRayMaxTravelDist(float dist)
{
	mRayMaxTravelDist = dist;
}

float Noise3D::GI::PathTracer::GetRayMaxTravelDist()
{
	return mRayMaxTravelDist;
}

bool Noise3D::GI::PathTracer::IsRenderFinished()
{
	return mIsRenderedFinished;
}

void Noise3D::GI::PathTracer::TerminateRenderTask()
{
	mIsRenderedFinished = true;
	for (int i = 0; i < c_parallelWorkerThreadCount; ++i)
	{
		if(mWorkerThreadArr[i].joinable())mWorkerThreadArr[i].join();
	}
}

void Noise3D::GI::PathTracer::SetExposure(float e)
{
	if (e < 0.0f)e = 0.0f;
	mLogExposurePreAmp = e;
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

	PathTracer::SetRenderTarget(pRenderTarget);
	return true;
}

void Noise3D::GI::PathTracer::mFunction_ComputeLightSourceList(std::vector<GI::IGiRenderable*>& outList)
{
	//gather all emssive objects' info, as light sources
	//local lighting at each closest hit is needed (to cast shadow ray to emissive object)
	const BvhTreeForGI& bvh = m_pCT->GetBvhTree();
	std::vector<GI::IGiRenderable*> objList;
	bvh.TraverseSceneObjects(NOISE_TREE_TRAVERSE_ORDER::PRE_ORDER, objList);

	//traverse all scene object, and see if their material's emission is black
	for (auto pRenderable : objList)
	{
		GI::AdvancedGiMaterial* pMat = nullptr;

		pMat = pRenderable->GetGiMaterial();

		if (pMat->IsEmissionEnabled())
		{
			outList.push_back(pRenderable);
		}
	}
}

Color4f Noise3D::GI::PathTracer::mFunction_ToneMapping(Color4f c)
{
	/*tmpColor *= mLogExposurePreAmp;
	Color4f logExposureColor = Color4f(
		log10f(tmpColor.x + 1.0f),
		log10f(tmpColor.y + 1.0f),
		log10f(tmpColor.z + 1.0f), 0.0f);*/

	float r = 1.0f - std::expf(-c.x * mLogExposurePreAmp);
	float g = 1.0f - std::expf(-c.y * mLogExposurePreAmp);
	float b = 1.0f - std::expf(-c.z * mLogExposurePreAmp);
	float a = 1.0f - std::expf(-c.w * mLogExposurePreAmp);
	return Color4f(r,g,b,a);
}

void Noise3D::GI::PathTracer::_RenderTileWorkerThread(std::queue<N_RenderTileInfo>& renderTaskList)
{
	//serveral worker thread works continuously, independently with no break.
	//all wait to be fed by new task. (
	//until all work are done
	while (true)
	{
		//if render process is manually forced to terminate, then quit immediately.
		if (mIsRenderedFinished)break;

		//lock mutex and retrieve undone job info from queue
		//but only one thread can access queue at the same time
		while (!mRenderTaskMutex.try_lock());
		if (renderTaskList.empty())break;
		N_RenderTileInfo info = renderTaskList.front();
		renderTaskList.pop();
		mRenderTaskMutex.unlock();

		//start to do real thing
		PathTracer::_RenderTile(std::move(info));
	}
	mRenderTaskMutex.unlock();
}

void Noise3D::GI::PathTracer::_RenderTile(const N_RenderTileInfo & info)
{
	Camera* pCam = Noise3D::GetScene()->GetCamera();
	for (uint32_t x = 0; x < info.width; ++x)
	{
		for (uint32_t y = 0; y < info.height; ++y)
		{
			uint32_t globalPixelX = info.topLeftX + x;
			uint32_t globalPixelY = info.topLeftY + y;
			uint32_t totalWidth = m_pFinalRenderTarget->GetWidth();
			uint32_t totalHeight = m_pFinalRenderTarget->GetHeight();

			//start tracing a ray with payload
			N_TraceRayPayload payload;
			N_TraceRayParam param;
			param.bounces = 0;
			param.travelledDistance = 0.0f;
			param.ray = pCam->FireRay_WorldSpace(PixelCoord2(float(globalPixelX), float(globalPixelY)),totalWidth, totalHeight);
			param.isInsideObject = false;
			param.isShadowRay = false;
			PathTracer::TraceRay(param, payload);

			//set one pixel at a time 
			//(it's ok, one pixel is not easy to evaluate, setpixel won't be a big overhead)
			Color4f tmpColor = Color4f(payload.radiance.x, payload.radiance.y, payload.radiance.z, 1.0f);
			mHdrRenderTarget.at(globalPixelY * totalWidth + globalPixelX) = tmpColor;

			//(2019.4.19)log exposure to remap hdr(?) (perhaps tone mapping later)
			Color4u outputColor(mFunction_ToneMapping(tmpColor));//convert to 8bitx4 color via constructor
			m_pFinalRenderTarget->SetPixel(info.topLeftX + x, info.topLeftY + y, outputColor);
		}
	}
}

void Noise3D::GI::PathTracer::TraceRay(const N_TraceRayParam& param, N_TraceRayPayload& out_payload)
{
	//initial bounces and travelled distance must remain in limit
	if (param.bounces > int(PathTracer::GetMaxBounces()) ||
		param.travelledDistance > PathTracer::GetRayMaxTravelDist())return;

	//intersect the ray with the scene and get results
	N_RayHitResultForPathTracer hitResult;
	m_pCT->IntersectRaySceneForPathTracer(param.ray, hitResult);

	//call shader. Radiance and other infos are carried by Payload reference.
	if (hitResult.HasAnyHit())
	{
		int index = hitResult.GetClosestHitIndex();
		N_RayHitInfoForPathTracer& info = hitResult.hitList.at(index);

		//update ray's travelled distance
		float newTravelledDistance = param.travelledDistance + param.ray.Distance(info.t);
		if (newTravelledDistance > PathTracer::GetRayMaxTravelDist())return;

		m_pShader->ClosestHit(param, info, out_payload);
	}
	else
	{
		m_pShader->Miss(param, out_payload);
	}

}
