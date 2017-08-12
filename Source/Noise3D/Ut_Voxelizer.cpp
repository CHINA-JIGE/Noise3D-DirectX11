/*********************************************************

						cpp: Voxelizer

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

bool IVoxelizer::Init(NFilePath STLModelFile, UINT cubeCountX, UINT cubeCountY, UINT cubeCountZ)
{
	mVoxelizedModel.Resize(cubeCountX, cubeCountY, cubeCountZ);

	//step1 - load model
	BOOL fileLoadSucceeded = mSlicer.Step1_LoadPrimitiveMeshFromSTLFile(STLModelFile);
	if (!fileLoadSucceeded)
	{
		ERROR_MSG("IVoxelizer: Init failed. Illegal file path");
		return false;
	}
	return true;
}

bool IVoxelizer::Init(const std::vector<NVECTOR3>& vertexList,const std::vector<UINT>& indexList, UINT cubeCountX, UINT cubeCountY, UINT cubeCountZ)
{
	mVoxelizedModel.Resize(cubeCountX, cubeCountY, cubeCountZ);

	//step1 - load model
	BOOL modelLoadSucceeded = mSlicer.Step1_LoadPrimitiveMeshFromMemory(vertexList,indexList);
	if (!modelLoadSucceeded)
	{
		ERROR_MSG("IVoxelizer: Init failed. Input model data is corrupted.");
		return false;
	}
	return true;
}


void IVoxelizer::Voxelize()
{
	//---------step2 - intersection---------------
	UINT layerCount = mVoxelizedModel.GetVoxelCountY();
	mSlicer.Step2_Intersection(layerCount);


	//get line segement buffer and then rasterize every line segment, 
	//pad inner area in the end
	std::vector<N_LayeredLineSegment2D> lineSegmentList;
	mSlicer.GetLineSegmentBuffer(lineSegmentList);


	//----------step3 - layer rasterization-------------
	//get bounding box to determine target voxelizing spatial area
	N_Box bbox = mSlicer.GetMeshAABB();
	mLayerPosMin = { bbox.min.x,bbox.min.z };
	mLayerPosMax = { bbox.max.x,bbox.max.z };
	mLayerRealWidth = mLayerPosMax.x - mLayerPosMin.x;
	mLayerRealDepth = mLayerPosMax.y - mLayerPosMin.y;

	mFunction_Rasterize(lineSegmentList);
}

void  IVoxelizer::GetVoxelizedModel(IVoxelizedModel& outModel)
{
	outModel = mVoxelizedModel;
}

/*******************************************************

									PRIVATE

*********************************************************/

void IVoxelizer::mFunction_Rasterize(const std::vector<N_LayeredLineSegment2D>& lineSegList)
{

	for (auto& line : lineSegList)
	{
		UINT cubeCountY = mVoxelizedModel.GetVoxelCountY();
		for (UINT i = 0; i < cubeCountY; ++i)
		{
			// layer pixel height = rows in a layer
			float normalized_scanlineY = ((float(i) + 0.5f) / cubeCountY);
			//scanline - lineSegment intersection
			mFunction_LineSegment_Scanline_Intersect(line, i, normalized_scanlineY);
		}
	}

	//intersect-points' X coord in each row should be sorted in order to use scan line padding algorithm
	for (auto& layer : mIntersectXCoordLayers)
	{
		for (auto & row : layer)
		{
			std::sort(row.begin(), row.end());
		}
	}

	//pad inner area
	for (UINT i = 0; i < mVoxelizedModel.GetVoxelCountY(); ++i)
	{
		mFunction_PadInnerArea(mIntersectXCoordLayers.at(i),i);
	}

}


void IVoxelizer::mFunction_LineSegment_Scanline_Intersect(const N_LayeredLineSegment2D& line, UINT scanlineRowID, float y)
{

	//v1,v2 are transformed into NORMALIZED space, valued in [0,1]
	//the point with minimum x,y coord is the origin point
	NVECTOR2 v1 = { (line.v1.x - mLayerPosMin.x) / mLayerRealWidth , (line.v1.y - mLayerPosMin.y) / mLayerRealDepth };
	NVECTOR2 v2 = { (line.v2.x - mLayerPosMin.x) / mLayerRealWidth , (line.v2.y - mLayerPosMin.y) / mLayerRealDepth };


	//line segment could come from any layer (with different Y coordinate)
	N_IntersectXCoordList& layerIntersectResult = mIntersectXCoordLayers.at(line.layerID);

	if ((v1.y > y && v2.y > y) || (v1.y < y && v2.y <y))
	{
		//there is no way scan line and line segment can intersect like this
		return;
	}



	// now y valued between v1.y and v2.y can be assured
	if (v1.y == v2.y)
	{
		//this is a very special case

		//v1.x and v2.x serve as the X region that can pad the line segment
		layerIntersectResult.at(scanlineRowID).push_back(v1.x);
		layerIntersectResult.at(scanlineRowID).push_back(v2.x);

		//this v2.x serve as the start X of next horizontal padding area
		layerIntersectResult.at(scanlineRowID).push_back(v2.x);
		return;
	}

	//vector ratio coeffient t
	float t = (y - v1.y) / (v2.y - v1.y);
	if (t >= 0.0f && t < 1.0f)
	{
		layerIntersectResult.at(scanlineRowID).push_back(v1.x + t * (v2.x - v1.x));
	}
	return;

}

//currently not in use (2017.1.19)
void IVoxelizer::mFunction_PadInnerArea(N_IntersectXCoordList& layer, UINT layerID)
{
	//Scan Line Padding , horizontal line scans from top to bottom
	for (UINT z = 0; z < layer.size(); ++z)
	{
		auto& XCoordRow = layer.at(z);
		//for every X coord pair (a region)
		for (UINT j = 0; j < XCoordRow.size(); j += 2)
		{
			//I don't know why some rows have odd intersect points....
			if (XCoordRow.size() - j == 1)break;

			//NOTE: accurate X coordinate of intersect points derived from
			//each scan line had been computed. now we only need quantize
			//these X coord and start padding from odd index to even index X coord

			UINT cubeCountX = mVoxelizedModel.GetVoxelCountX();
			UINT startX = UINT(XCoordRow.at(j)  * float(cubeCountX));
			UINT endX = UINT(XCoordRow.at(j + 1)*float(cubeCountX));


			//scan line padding : pad from left to right
			for (UINT x = startX; x <= endX; ++x)
			{
				mVoxelizedModel.SetVoxel(true,x, layerID, z);
			}

		}

	}

}

