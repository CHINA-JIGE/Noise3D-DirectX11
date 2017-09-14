/*********************************************************

						cpp: Voxelizer

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

IVoxelizer::IVoxelizer():
	mIsInitialized(false)
{
}

bool IVoxelizer::Init(NFilePath STLModelFile, uint16_t cubeCountX, uint16_t cubeCountY, uint16_t cubeCountZ)
{

	mIntersectXCoordLayers.resize(cubeCountY);
	for (auto& layer : mIntersectXCoordLayers)
	{
		layer.resize(cubeCountZ);
	}

	//step1 - load model
	bool fileLoadSucceeded = mSlicer.Step1_LoadPrimitiveMeshFromSTLFile(STLModelFile);
	N_Box bbox = mSlicer.GetBoundingBox();
	float width = bbox.max.x - bbox.min.x;
	float height = bbox.max.y - bbox.min.y;
	float depth = bbox.max.z - bbox.min.z;
	mVoxelizedModel.Resize(cubeCountX, cubeCountY, cubeCountZ,width/float(cubeCountX) , height / float(cubeCountY), depth / float(cubeCountZ));
	if (!fileLoadSucceeded)
	{
		ERROR_MSG("IVoxelizer: Init failed. Illegal file path");
		return false;
	}
	mIsInitialized = true;
	return true;
}

bool IVoxelizer::Init(const std::vector<NVECTOR3>& vertexList,const std::vector<UINT>& indexList, UINT cubeCountX, UINT cubeCountY, UINT cubeCountZ, float cubeWidth, float cubeHeight, float cubeDepth)
{
	mVoxelizedModel.Resize(cubeCountX, cubeCountY, cubeCountZ, cubeWidth, cubeHeight, cubeDepth);

	//step1 - load model
	bool modelLoadSucceeded = mSlicer.Step1_LoadPrimitiveMeshFromMemory(vertexList,indexList);
	if (!modelLoadSucceeded)
	{
		ERROR_MSG("IVoxelizer: Init failed. Input model data is corrupted.");
		return false;
	}
	mIsInitialized = true;
	return true;
}


void IVoxelizer::Voxelize()
{
	if (!mIsInitialized)
	{
		ERROR_MSG("IVoxelizer: not initialized!");
		return;
	}

	//---------step2 - intersection---------------
	UINT layerCount = mVoxelizedModel.GetVoxelCountY();
	mSlicer.Step2_Intersection(layerCount);


	//get line segement buffer and then rasterize every line segment, 
	//pad inner area in the end
	std::vector<N_LayeredLineSegment2D> lineSegmentList;
	mSlicer.GetLineSegmentBuffer(lineSegmentList);


	//----------step3 - layer rasterization-------------
	//get bounding box to determine target voxelizing spatial area
	N_Box bbox = mSlicer.GetBoundingBox();
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
	//1.
	for (auto& line : lineSegList)
	{
		UINT cubeCountZ = mVoxelizedModel.GetVoxelCountZ();

		//normalized y (on 2d plane)
		float y1_2d = (line.v1.y - mLayerPosMin.y) / mLayerRealDepth ;
		float y2_2d = (line.v2.y - mLayerPosMin.y) / mLayerRealDepth ;
		UINT startY_2d = UINT((min(y1_2d, y2_2d)) * cubeCountZ);
		UINT endY_2d = (UINT((max(y1_2d, y2_2d)) * cubeCountZ)+1);
		if (endY_2d >= cubeCountZ)endY_2d = cubeCountZ - 1;//boundary check

		for (UINT i = startY_2d; i < endY_2d; ++i)
		{
			// layer pixel height = rows in a layer
			float normalized_scanlineY = ((float(i)) / cubeCountZ);
			//scanline - lineSegment intersection
			mFunction_LineSegment_Scanline_Intersect(line, i, normalized_scanlineY);
		}
	}

	//2.  re-calculate ambiguous situation by approximation
	for (auto& a : mAmbiguousIntersection)
	{
		//clear the whole row,re-calculate
		mIntersectXCoordLayers.at(a.layerID).at(a.scanLineRowID).clear();
		//an offset added to y, trying to approximate the filling situation while
		//avoiding ambiguous situation
		a.originalScanlineYCoord += 0.001f;
		//acitve line: possible intersection
		for (auto& activeLine : a.activeLineSegmentList)
		{
			//new ambiguous situation could be generated, loop until all amb-situation are resolved by approximation
			mFunction_LineSegment_Scanline_Intersect(activeLine, a.scanLineRowID, a.originalScanlineYCoord);
		}
	}

	//3. intersect-points' X coord in each row should be sorted in order to use scan line padding algorithm
	for (auto& layer : mIntersectXCoordLayers)
	{
		for (auto & row : layer)
		{
			std::sort(row.begin(), row.end());
		}
	}

	//4. pad inner area of each layer
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
	NVECTOR2 v2 = {(line.v2.x - mLayerPosMin.x) / mLayerRealWidth , (line.v2.y - mLayerPosMin.y) / mLayerRealDepth };


	//line segment could come from any layer (with different Y coordinate)
	N_IntersectXCoordList& layerIntersectResult = mIntersectXCoordLayers.at(line.layerID);

	if ((v1.y > y && v2.y > y) || (v1.y < y && v2.y <y))
	{
		//there is no way scan line and line segment can intersect like this
		return;
	}

	// assuring no vertex of line segment are on the scanline
	if ( v1.y==y || v2.y==y)
	{
		//this is a very special case, actually in this case 
		//filling behaviour could be UN-DEFINED.

		//try to find existed Ambiguous situation and only add new active line to it
		for (auto& asc : mAmbiguousIntersection)
		{
			if (asc.layerID == line.layerID && asc.scanLineRowID == scanlineRowID)
			{
				asc.activeLineSegmentList.push_back(line);
				return;
			}
		}

		//new ambiguous scan line
		N_AmbiguousScanlineCircumstance newAsc;
		newAsc.activeLineSegmentList.push_back(line);
		newAsc.layerID = line.layerID;
		newAsc.scanLineRowID = scanlineRowID;
		newAsc.originalScanlineYCoord = y;
		mAmbiguousIntersection.push_back(newAsc);

		/*layerIntersectResult.at(scanlineRowID).push_back(v1.x);
		layerIntersectResult.at(scanlineRowID).push_back(v2.x);
		layerIntersectResult.at(scanlineRowID).push_back(v2.x);*/
		return;
	}

	//vector ratio coeffient t
	float t = (y - v1.y) / (v2.y - v1.y);
	if (t > 0.0f && t < 1.0f)
	{
		layerIntersectResult.at(scanlineRowID).push_back(v1.x + t * (v2.x - v1.x));
	}
	return;

}


void IVoxelizer::mFunction_PadInnerArea(N_IntersectXCoordList& layer, UINT layerID)
{
	//Scan Line Padding , horizontal line scans from top to bottom
	UINT cubeCountX = mVoxelizedModel.GetVoxelCountX();
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
			UINT startX = UINT(XCoordRow.at(j)  * float(cubeCountX));
			UINT endX = UINT(XCoordRow.at(j + 1)*float(cubeCountX));


			//scan line padding : pad from left to right
			/*for (UINT x = startX; x < endX; ++x)
			{
				mVoxelizedModel.SetVoxel(1,x, layerID, z);
			}*/
			mVoxelizedModel.SetVoxel(1, startX, endX, layerID, z);

		}

	}

}

