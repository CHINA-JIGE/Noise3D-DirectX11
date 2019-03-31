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
	mTileHeight(16)
{
}

Noise3D::GI::PathTracer::~PathTracer()
{
	m_pRenderTarget = nullptr;
}

void Noise3D::GI::PathTracer::Render(Noise3D::SceneNode * pNode, IPathTracerSoftShader * pShader)
{
	if (pNode == nullptr || pShader==nullptr)
	{
		WARNING_MSG("Path Tracer: parameter ptr is nullptr. nothing will be rendered.");
		return;
	}

	//re-build BVH to accelerate ray-object intersection
	CollisionTestor* pCT = Noise3D::GetScene()->GetCollisionTestor();
	pCT->RebuildBvhTree(pNode);

	//set soft shader
	m_pShader = pShader;

	//back buffer's size, partition it into tiles
	uint32_t h = m_pRenderTarget->GetWidth();
	uint32_t w = m_pRenderTarget->GetHeight();
	uint32_t completeTileCountX = (w / mTileWidth);
	uint32_t completeTileCountY = (h / mTileHeight);
	uint32_t partialTileWidth = w % mTileWidth;//some tile might be incomplete
	uint32_t partialTileHeight = h % mTileHeight;//some tile might be incomplete

	//dispatch renderTile() task, could be parallelized using multi-thread
	for (uint32_t tileIdY = 0; tileIdY <= completeTileCountY; ++tileIdY)
	{
		for (uint32_t tileIdX = 0; tileIdX <= completeTileCountX; ++tileIdX)
		{
			//incomplete block at the right/bottom edeg are considered
			uint32_t currentTileW = (tileIdX == completeTileCountX ? mTileWidth : partialTileWidth);
			uint32_t currentTileH = (tileIdY == completeTileCountY ? mTileHeight : partialTileHeight);

			N_RenderTileInfo info;
			info.topLeftX = mTileWidth * tileIdX;
			info.topLeftY = mTileHeight * tileIdY;
			info.width = currentTileW;
			info.height = currentTileH;

			//ignore edge tile with no pixel
			if (currentTileW>0 && currentTileH >0)
			{
				PathTracer::RenderTile(info);
			}
		}
	}

}

void Noise3D::GI::PathTracer::SetRenderTileSize(uint32_t width, uint32_t height)
{
	mTileWidth = width > 0 ? width : 1;
	mTileHeight = height > 0 ? height : 1;
}

ITexture * Noise3D::GI::PathTracer::GetRenderTarget()
{
	return m_pRenderTarget;
}

void Noise3D::GI::PathTracer::SetRenderTarget(ITexture * pRenderTarget)
{
	if(pRenderTarget!=nullptr)m_pRenderTarget = pRenderTarget;
}

void Noise3D::GI::PathTracer::SetBounces(uint32_t bounces)
{
	mBounces = bounces;
}

uint32_t Noise3D::GI::PathTracer::GetBounces()
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
