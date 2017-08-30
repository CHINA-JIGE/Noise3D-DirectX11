
/***********************************************************************

										类：Ut::Slicer

				简述：用于生成3D打印所需的 切层矢量数据

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

IMeshSlicer::IMeshSlicer():
	mBoundingBox_Min(0, 0, 0),
	mBoundingBox_Max(0, 0, 0),
	mCurrentStep(0)
{

}

bool IMeshSlicer::Step1_LoadPrimitiveMeshFromMemory(const std::vector<NVECTOR3>& vb, const std::vector<UINT>& ib)
{
	mCurrentStep = 1;

	mLayerList.clear();
	mPrimitiveVertexBuffer.clear();
	mTriangleNormalBuffer.clear();
	mLineSegmentBuffer.clear();
	mLineStripBuffer.clear();

	if (vb.size() == 0)
	{
		return false;
	}

	//convert ib&vb into a single triangle list
	for (UINT i = 0; i < ib.size(); i++)
	{
		UINT index = ib.at(i);
		if (index >= vb.size())
		{
			ERROR_MSG("MeshSlicer : Illegal index found in index list!");
			return false;
		}

		mPrimitiveVertexBuffer.push_back(vb.at(ib.at(i)));
	}

	//do it right after loading the file
	mFunction_ComputeBoundingBox();

	mCurrentStep = 2;
	return true;
}

bool IMeshSlicer::Step1_LoadPrimitiveMeshFromMemory(const std::vector<N_DefaultVertex>& vb)
{
	mCurrentStep = 1;

	mLayerList.clear();
	mPrimitiveVertexBuffer.clear();
	mTriangleNormalBuffer.clear();
	mLineSegmentBuffer.clear();
	mLineStripBuffer.clear();

	if (vb.size() == 0)
	{
		return false;
	}

	for (UINT i = 0;i < vb.size();i++)
	{
		mPrimitiveVertexBuffer.push_back(vb.at(i).Pos);
	}
	
	//do it right after loading the file
	mFunction_ComputeBoundingBox();

	mCurrentStep = 2;
	return true;
}

bool IMeshSlicer::Step1_LoadPrimitiveMeshFromSTLFile(NFilePath pFilePath)
{
	mCurrentStep = 1;

	// this function is used to load STL file , and Primitive Vertex Data has 3x more elements
	//than Triangle Normal Buffer , this is because 1 triangle consists of 3 vertices.

	std::vector<UINT>	tmpIndexBuffer;
	std::string					tmpHeaderString;
	bool isLoadSTLsucceeded = false;

	//index buffer is dumped because there is actually no "index" in STL model file
	isLoadSTLsucceeded =IFileIO::ImportFile_STL(
		pFilePath,
		mPrimitiveVertexBuffer, 
		tmpIndexBuffer,
		mTriangleNormalBuffer, 
		tmpHeaderString);

	//assert
	if (!isLoadSTLsucceeded)
	{
		ERROR_MSG("MeshSlicer: Load STL file failed!!");
		return false;
	}

	//do it right after loading the file
	mFunction_ComputeBoundingBox();

	mCurrentStep = 2;
	return true;
}

void IMeshSlicer::Step2_Intersection(UINT iLayerCount)
{
	if (mCurrentStep != 2)
	{
		ERROR_MSG("MeshSlicer : Model hasn't been initialized");
		return;
	}

	//Objective of this function is GENERALLY  to generate Line segments ,sometimes directly add triangles

	if (iLayerCount < 2)
	{
		ERROR_MSG("MeshSlicer : Layer Count is too little !!");
	}

	//top/bottom y of the bounding box (AABB)
	float modelMinY = mBoundingBox_Min.y;
	float modelMaxY = mBoundingBox_Max.y;


	//calculate  delta Y between layers , note that the TOP and BOTTOM are taken into consideration
	//thus ,  minus 1
	float  layerDeltaY = (modelMaxY - modelMinY) / (float)(iLayerCount-1);

	//since in one intersection request , different thickness settings might be used for a Y region
	//so we must accumulate LayerCount of ALL intersection mission;
	UINT totalLayerCount = iLayerCount;

	//the Y coord of current layer
	float		currentLayerY = 0;

	//how many vertex of a single triangle are on this layer ,valued 0,1,2,3.
	//if vertex count ==0 , we should see if the layer can proceed further intersection with edges
	//which is stored in the "N_IntersectionResult"
	N_IntersectionResult	tmpResult;

	//Total Triangle  Count (one Normal Vector correspond to one Triangle
	UINT		totalTriangleCount = mTriangleNormalBuffer.size();

	//........tmp var to store 3 vertex of triangle


	//.....
	N_LayeredLineSegment tmpLineSegment;

	//...tmp var  : used in  "switch (tmpResult.mVertexCount)"
	std::vector<NVECTOR3> tmpIntersectPointList;
	bool canIntersect = false;
	NVECTOR3 tmpPoint(0, 0, 0);


	//start traverse all layers / triangles , and intersect
	//for (currentLayerID = 0; currentLayerID < iLayerCount;currentLayerID++)
	for (UINT currentTriangleID = 0; currentTriangleID <totalTriangleCount; currentTriangleID++)
	{

		NVECTOR3 v1 = NVECTOR3(0, 0, 0),v2 = NVECTOR3(0, 0, 0),v3 = NVECTOR3(0, 0, 0);
		v1 = mPrimitiveVertexBuffer.at(currentTriangleID * 3 + 0);
		v2 = mPrimitiveVertexBuffer.at(currentTriangleID * 3 + 1);
		v3 = mPrimitiveVertexBuffer.at(currentTriangleID * 3 + 2);

		float triangleMinY = min(min(v1.y, v2.y), v3.y);
		float triangleMaxY = max(max(v1.y, v2.y), v3.y);
		UINT startLayer = UINT((triangleMinY - modelMinY) / layerDeltaY);
		UINT endLayer = UINT((triangleMaxY - modelMinY) / layerDeltaY)+1;

		//calculate how many vertex of this triangle are on this layer
		//for (currentTriangleID = 0;currentTriangleID <totalTriangleCount;  currentTriangleID++)
		for (UINT currentLayerID = startLayer; currentLayerID < endLayer; currentLayerID++)
		{
			currentLayerY = modelMinY + layerDeltaY * ((float)currentLayerID);

			// tmpResult:"N_IntersectionResult" 
			tmpResult = mFunction_HowManyVertexOnThisLayer(currentLayerY,v1,v2,v3);

			//Category Discussion	(?分类讨论?)
			switch (tmpResult.mVertexCount)
			{
			//-------------------------------
			case 0:
#pragma region VertexOnLayer : 0

				if (tmpResult.isPossibleToIntersectEdges)
				{
					//maybe some edges will intersect current Layer
					bool canIntersect = false;
					NVECTOR3 tmpPoint(0, 0, 0);

					//proceed a line --- layer intersection ,and return a bool
					//if they really intersect so we add the intersect point to a list
					canIntersect = mFunction_Intersect_LineSeg_Layer(v1, v2, currentLayerY, &tmpPoint);
					if (canIntersect){ tmpIntersectPointList.push_back(tmpPoint);}

					canIntersect = mFunction_Intersect_LineSeg_Layer(v1, v3, currentLayerY, &tmpPoint);
					if (canIntersect) { tmpIntersectPointList.push_back(tmpPoint); }

					canIntersect = mFunction_Intersect_LineSeg_Layer(v2, v3, currentLayerY, &tmpPoint);
					if (canIntersect) { tmpIntersectPointList.push_back(tmpPoint); }

					//theoretically , intersectPoint will only got 2 members , but maybe shit happens ??
					if (tmpIntersectPointList.size() == 2)
					{
						N_LayeredLineSegment tmpLineSegment;
						tmpLineSegment.v1 = tmpIntersectPointList.at(0);
						tmpLineSegment.v2 = tmpIntersectPointList.at(1);
						tmpLineSegment.LayerID = currentLayerID;
						tmpLineSegment.Dirty = false;
						//triangle normal projection , look for tech doc for more detail
						tmpLineSegment.normal = mFunction_Compute_Normal2D(mTriangleNormalBuffer.at(currentTriangleID));
						mLineSegmentBuffer.push_back(tmpLineSegment);//add to disordered line segments buffer
					}


					// clear the tmpIntersectPoint Buffer
					tmpIntersectPointList.clear();
				}

#pragma endregion 
				break;

			//-------------------------------
			case 1:
#pragma region VertexOnLayer : 1

				//if one point is on the layer ,then the line segment composed of other 2 points -
				//will try to intersect the layer
				switch (tmpResult.mIndexList.at(0))
				{
					//see which point is on this layer
				case 0:
					tmpIntersectPointList.push_back(v1);
					canIntersect = mFunction_Intersect_LineSeg_Layer(v2, v3, currentLayerY, &tmpPoint);
					if (canIntersect) { tmpIntersectPointList.push_back(tmpPoint); }
					break;
					//see which point is on this layer

				case 1:
					tmpIntersectPointList.push_back(v2);
					canIntersect = mFunction_Intersect_LineSeg_Layer(v1, v3, currentLayerY, &tmpPoint);
					if (canIntersect) { tmpIntersectPointList.push_back(tmpPoint); }
					break;
					//see which point is on this layer

				case 2:
					tmpIntersectPointList.push_back(v3);
					canIntersect = mFunction_Intersect_LineSeg_Layer(v1, v2, currentLayerY, &tmpPoint);
					if (canIntersect) { tmpIntersectPointList.push_back(tmpPoint); }
					break;
				}

				//2 intersect point will make up a line segment
				if (tmpIntersectPointList.size() == 2)
				{
					N_LayeredLineSegment tmpLineSegment;
					tmpLineSegment.v1 = tmpIntersectPointList.at(0);
					tmpLineSegment.v2 = tmpIntersectPointList.at(1);
					tmpLineSegment.LayerID = currentLayerID;
					tmpLineSegment.Dirty = false;
					//triangle normal projection , look for tech doc for more detail
					tmpLineSegment.normal = mFunction_Compute_Normal2D(mTriangleNormalBuffer.at(currentTriangleID));
					mLineSegmentBuffer.push_back(tmpLineSegment);//add to disordered line segments buffer
				}


				// clear the tmpIntersectPoint Buffer
				tmpIntersectPointList.clear();


#pragma endregion 
				break;

			//-------------------------------
			case 2:
#pragma region VertexOnLayer : 2
				//v1,v2 are on this layer ,so just directly add to line segment buffer
				//....the first vertex
				switch (tmpResult.mIndexList.at(0))
				{
				case 0:
					tmpIntersectPointList.push_back(v1);
					break;
				case 1:
					tmpIntersectPointList.push_back(v2);
					break;
				case 2:
					tmpIntersectPointList.push_back(v3);
					break;
				}

				//...the second vertex (use  index to determine which vertex should we add)
				switch (tmpResult.mIndexList.at(1))
				{
				case 0:
					tmpIntersectPointList.push_back(v1);
					break;
				case 1:
					tmpIntersectPointList.push_back(v2);
					break;
				case 2:
					tmpIntersectPointList.push_back(v3);
					break;
				}

				//theoretically , intersectPoint will only got 2 members , but maybe shit happens ??
				if (tmpIntersectPointList.size() == 2)
				{
					N_LayeredLineSegment tmpLineSegment;
					tmpLineSegment.v1 = tmpIntersectPointList.at(0);
					tmpLineSegment.v2 = tmpIntersectPointList.at(1);
					tmpLineSegment.LayerID = currentLayerID;
					tmpLineSegment.Dirty = false;
					//triangle normal projection , look for tech doc for more detail
					tmpLineSegment.normal = mFunction_Compute_Normal2D(mTriangleNormalBuffer.at(currentTriangleID));
					mLineSegmentBuffer.push_back(tmpLineSegment);//add to disordered line segments buffer
				}

				// clear the tmpIntersectPoint Buffer
				//tmpIntersectPointList.erase(tmpIntersectPointList.begin(), tmpIntersectPointList.end());
				tmpIntersectPointList.clear();

#pragma endregion 
				break;

			//-------------------------------
			default:
				break;
			}
		}

	}

	//preparation for next step
	mLayerList.resize(totalLayerCount);

	mCurrentStep = 3;
}

void	IMeshSlicer::Step3_GenerateLineStrip()
{
	if (mCurrentStep != 3)
	{
		ERROR_MSG("MeshSlicer : step 2 was not executed. ");
		return;
	}

	//preprocess (generate layer tile information , optimization for linking line segment)
	mFunction_GenerateLayerTileInformation();

	//link numerous line segments into line strip
	//(and at the present  ignore the problem of  'multiple branches'

	N_LineStrip			tmpLineStrip;
	N_LayeredLineSegment		tmpLineSegment;
	NVECTOR3			tmpLineStripTailPoint;

	//............
	UINT i = 0, j = 0;

	bool canFindNextPoint = false;

	for (i = 0;i < mLineSegmentBuffer.size(); i++)
	{

		tmpLineSegment = mLineSegmentBuffer.at(i);
		//find the first line segment valid to be the head of line strip
		if (tmpLineSegment.Dirty == false)
		{
				//we have found a "clean" line segment , then add 2 vertices to the current line strip
				//this is a new line strip spawned
				//v2 is the tail of the strip
				tmpLineStrip.LayerID = tmpLineSegment.LayerID;
				tmpLineStrip.pointList.push_back(tmpLineSegment.v1);
				tmpLineStrip.pointList.push_back(tmpLineSegment.v2);
				tmpLineStrip.normalList.push_back(tmpLineSegment.normal);
				tmpLineStripTailPoint = tmpLineSegment.v2;
		}
		else
		{
			goto nextLineSegment;
		}

		//make the new  line strip grow longer until no more line segment can be added to the tail
		canFindNextPoint = mFunction_LineStrip_FindNextPoint(&tmpLineStripTailPoint, tmpLineStrip.LayerID, &tmpLineStrip);
		while (canFindNextPoint)
		{
			canFindNextPoint = mFunction_LineStrip_FindNextPoint(&tmpLineStripTailPoint, tmpLineStrip.LayerID, &tmpLineStrip);
		}

		//we have finished growing a line strip, so add it to line Strip Buffer;
		mLineStripBuffer.push_back(tmpLineStrip);

		//clear the tmp buffer so that we can continue processing new line strip
		tmpLineStrip.pointList.clear();
		tmpLineStrip.normalList.clear();

	nextLineSegment:;
	}//for i

	mCurrentStep = 4;
}

bool IMeshSlicer::Step3_LoadLineStripsFrom_NOISELAYER_File(char * filePath)
{
	bool isSucceeded;
	isSucceeded = mFunction_ImportFile_NOISELAYER(filePath, &mLineStripBuffer);
	if (isSucceeded)mCurrentStep = 4;
	return isSucceeded;
}

bool IMeshSlicer::Step4_SaveLayerDataToFile(NFilePath filePath)
{
	if (mCurrentStep != 4)
	{
		ERROR_MSG("MeshSlicer : step3 was not executed.")
	}

	bool isSucceeded;  
	isSucceeded = mFunction_ExportFile_NOISELAYER(filePath, &mLineStripBuffer, true);
	if (isSucceeded)mCurrentStep = 5;//the end
	return isSucceeded;
}

UINT IMeshSlicer::GetLineSegmentCount()
{
	return mLineSegmentBuffer.size();
}

void IMeshSlicer::GetLineSegmentBuffer(std::vector<N_LayeredLineSegment2D>& outBuffer)
{
	outBuffer.clear();
	outBuffer.reserve(mLineSegmentBuffer.size());

	for (UINT i = 0;i < mLineSegmentBuffer.size();i++)
	{
		N_LayeredLineSegment2D outLineSeg;
		auto& line = mLineSegmentBuffer.at(i);
		outLineSeg.v1 = { line.v1.x,line.v1.z };
		outLineSeg.v2 = { line.v2.x,line.v2.z };
		outLineSeg.layerID = line.LayerID;
		outBuffer.push_back(outLineSeg);
	}

	/*for (i = 0;i < mLineStripBuffer.size();i++)
	{
		for (j = 0;j < mLineStripBuffer.at(i).pointList.size()-1;j++)
		{
			outBuffer.push_back(mLineStripBuffer.at(i).pointList.at(j));
			outBuffer.push_back(mLineStripBuffer.at(i).pointList.at(j+1));
		}
	}*/

}

UINT IMeshSlicer::GetLineStripCount()
{
	return mLineStripBuffer.size();
}

void IMeshSlicer::GetLineStrip(std::vector<N_LineStrip>& outPointList, UINT index)
{
	if (index < mLineStripBuffer.size())
	{
		outPointList.assign(	mLineStripBuffer.begin(),
										mLineStripBuffer.end());
	}
}

N_Box IMeshSlicer::GetBoundingBox()
{
	return N_Box(mBoundingBox_Min,mBoundingBox_Max);
}



/************************************************************************
											P R I V A T E
************************************************************************/
void		IMeshSlicer::mFunction_ComputeBoundingBox()
{
	//compute Bounding box : override 1

	UINT i = 0;
	NVECTOR3 tmpV;
	//traverse all vertices, and get the biggest and smallest point in terms of x,y,z components
	//遍历所有顶点，算出包围盒3分量均最 小/大 的两个顶点
	for (i = 0;i < mPrimitiveVertexBuffer.size();i++)
	{
		tmpV = mPrimitiveVertexBuffer.at(i);
		if (tmpV.x <(mBoundingBox_Min.x)) { mBoundingBox_Min.x = tmpV.x; }
		if (tmpV.y <(mBoundingBox_Min.y)) { mBoundingBox_Min.y = tmpV.y; }
		if (tmpV.z <(mBoundingBox_Min.z)) { mBoundingBox_Min.z = tmpV.z; }

		if (tmpV.x >(mBoundingBox_Max.x)) { mBoundingBox_Max.x = tmpV.x; }
		if (tmpV.y >(mBoundingBox_Max.y)) { mBoundingBox_Max.y = tmpV.y; }
		if (tmpV.z >(mBoundingBox_Max.z)) { mBoundingBox_Max.z = tmpV.z; }
	}

}

bool	IMeshSlicer::mFunction_Intersect_LineSeg_Layer(NVECTOR3 v1, NVECTOR3 v2, float layerY, NVECTOR3 * outIntersectPoint)
{

	//some obvious wrong input check
	if (outIntersectPoint==nullptr){return false;}
	if (v1.y == v2.y) { return false; }

	//init the out result
	ZeroMemory(outIntersectPoint, sizeof(*outIntersectPoint));


	//...detect if it is possible to intersect , according to "Zero Point Existance Theorem(?)"
	if ((v1.y > layerY && v2.y > layerY) || (v1.y < layerY && v2.y < layerY))
	{
		//impossible to intersect ,so just return
		return false;
	}

	//an interpolating ratio between these 2 points ,valued [-1,1]
	//vector start point is v2
	float lambda = (layerY - v2.y) / (v1.y - v2.y);

	//actually not an essential check,but of no harm = =
	if (lambda > -1.0f || lambda < 1.0f)
	{
		//interpolate
		*outIntersectPoint = v2 + lambda * (v1 - v2);

		//to avoid floating point number error
		outIntersectPoint->y = layerY;

		//...
		return true;
	}

	return false;
}

void		IMeshSlicer::mFunction_GenerateLayerTileInformation()
{
	//preprocess , generate line segment Info for each layer tile for optimization
	// if 1 or 2 of the line segment vertex is(are) right on the tile, then line seg ID will be recorded in a std::vector of this tile

	for (UINT i = 0;i < mLineSegmentBuffer.size();i++)
	{
		UINT currentLayerID = mLineSegmentBuffer.at(i).LayerID;
		N_LayeredLineSegment currentLineSeg = mLineSegmentBuffer.at(i);
		N_LineSegmentVertex tmpLineSegmentVertex;
		UINT tileID_X = 0, tileID_Z = 0;

#pragma region Line Segment Vertex 1
		//generate layer tile info for vertex 1 of current line segment

		//get 2 array index integers from point
		mFunction_GetLayerTileIDFromPoint(currentLineSeg.v1, tileID_X, tileID_Z);

		//add vertex/line segment info to corresponding layer tile
		tmpLineSegmentVertex.lineSegmentID = i;
		tmpLineSegmentVertex.vertexID = 1;
		//note that this is a 2D array !!!!!! dont let [] become overloaded operation of VECTOR
		//mLayerList.at(currentLayerID).layerTile	.at(tileID_X).at(tileID_Z).push_back(tmpLineSegmentVertex);
		mLayerList.at(currentLayerID).layerTile[tileID_X][tileID_Z].push_back(tmpLineSegmentVertex);


#pragma endregion Line Segment Vertex 1

#pragma region Line Segment Vertex 2
		//generate layer tile info for vertex 2 of current line segment

		//get 2 array index integers from point
		mFunction_GetLayerTileIDFromPoint(currentLineSeg.v2, tileID_X, tileID_Z);

		//add vertex/line segment info to corresponding layer tile
		tmpLineSegmentVertex.lineSegmentID = i;
		tmpLineSegmentVertex.vertexID = 2;
		mLayerList.at(currentLayerID).layerTile[tileID_X][tileID_Z].push_back(tmpLineSegmentVertex);
	
#pragma endregion Line Segment Vertex 2
}

}

void		IMeshSlicer::mFunction_GetLayerTileIDFromPoint(NVECTOR3 v, UINT & tileID_X, UINT & tileID_Z)
{
	//this function will return index(2 integers) that can be used to locate element in 2D array

	//bounding box of mesh has been computed in slicer- step1

	//axis-aligned bounding box (for the time being, we use bounding box to generate bounding sqare
	float MIN_X = mBoundingBox_Min.x;
	float MAX_X = mBoundingBox_Max.x;
	float MIN_Z = mBoundingBox_Min.z;
	float MAX_Z = mBoundingBox_Max.z;

	//map x to [0,1]first , then map to integer, and apply floot() operation
	 tileID_X = (UINT)floorf((v.x - MIN_X) / (MAX_X - MIN_X)*c_LayerTileStepCount);
	 tileID_Z = (UINT)floorf((v.z - MIN_Z) / (MAX_Z - MIN_Z)*c_LayerTileStepCount);

	 //...deal with boundary problem (the most right vertex on the edge)
	 if (tileID_X == c_LayerTileStepCount)tileID_X--;
	 if (tileID_Z == c_LayerTileStepCount)tileID_Z--;
};

IMeshSlicer::N_IntersectionResult	IMeshSlicer::mFunction_HowManyVertexOnThisLayer( float currentlayerY, NVECTOR3& v1, NVECTOR3& v2, NVECTOR3& v3)
{
	N_IntersectionResult outResult;

	//if all the vertex are beyond / below the layer
	bool b1 = (v1.y > currentlayerY) && (v2.y > currentlayerY) && (v3.y > currentlayerY);
	bool b2 = (v1.y < currentlayerY) && (v2.y < currentlayerY) && (v3.y < currentlayerY);
	if (b1 || b2 )
	{
		outResult.mVertexCount = 0;
		outResult.isPossibleToIntersectEdges = false;
		return outResult;
	}

	//if none of the vertex is on this layer
	if ((v1.y != currentlayerY) && (v2.y != currentlayerY) && (v3.y != currentlayerY) )
	{
		outResult.mVertexCount = 0;
		outResult.isPossibleToIntersectEdges = true;
		return outResult;
	}


	//note that error exist in float number , 0.001 is a threshold
	const float FLOAT_EQUAL_THRESHOLD = 0.001f;

	//count how many Vertices are on this layer
	if(abs(v1.y - currentlayerY) < FLOAT_EQUAL_THRESHOLD)
	{
		outResult.mVertexCount += 1;
		outResult.mIndexList.push_back(0);
	}

	if (abs(v2.y - currentlayerY) < FLOAT_EQUAL_THRESHOLD)
	{
		outResult.mVertexCount += 1;
		outResult.mIndexList.push_back(1);
	}

	if (abs(v3.y - currentlayerY) < FLOAT_EQUAL_THRESHOLD)
	{
		outResult.mVertexCount += 1;
		outResult.mIndexList.push_back(2);
	}

	return outResult;
}

bool IMeshSlicer::mFunction_LineStrip_FindNextPoint(NVECTOR3*  tailPoint, UINT currentLayerID, N_LineStrip* currLineStrip)
{
	//........used to judge if two point can be weld together
	float						tmpPointDist = 0;
	const float			SAME_POINT_DIST_THRESHOLD = 0.001f;
	NVECTOR3			tmpV;
	N_LayeredLineSegment		tmpLineSegment;

	UINT tileID_X = 0, tileID_Z = 0;

	//locate layer tile where the tail point lies in ( where it can find the next vertex to weld)
	mFunction_GetLayerTileIDFromPoint(*tailPoint, tileID_X, tileID_Z);

	//get layer tile
	auto currLayerTile = mLayerList.at(currentLayerID).layerTile[tileID_X][tileID_Z];

	for (UINT j = 0; j < currLayerTile.size();j++)
	{
		//read info from the tile , retrive and traverse vertices in this tile
		UINT lineSegID = currLayerTile.at(j).lineSegmentID;
		tmpLineSegment = mLineSegmentBuffer.at(lineSegID);

		//if this line segment has not been checked &&
		//the line segment is on the same layer as the stretching line strip
		if (tmpLineSegment.Dirty == false )
		{

			//if we can weld v1 to line strip tail 
			tmpV = *tailPoint - tmpLineSegment.v1;
			if (D3DXVec3Length(&tmpV) < SAME_POINT_DIST_THRESHOLD)
			{
				currLineStrip->pointList.push_back(tmpLineSegment.v2);
				currLineStrip->normalList.push_back(tmpLineSegment.normal);
				*tailPoint = tmpLineSegment.v2;
				//this line segment has been checked , so light up the DIRTY mark.
				mLineSegmentBuffer.at(lineSegID).Dirty = true;
				return true;
			}
			

			//if we can weld v2 to line strip tail 
			tmpV = *tailPoint - tmpLineSegment.v2;
			if (D3DXVec3Length(&tmpV) < SAME_POINT_DIST_THRESHOLD)
			{
				currLineStrip->pointList.push_back(tmpLineSegment.v1);
				currLineStrip->normalList.push_back(tmpLineSegment.normal);
				*tailPoint = tmpLineSegment.v1;
				//this line segment has been checked , so light up the DIRTY mark.
				mLineSegmentBuffer.at(lineSegID).Dirty = true;
				return true;
			}
		}

	}//for j

	//didn't find qualified line segment to link
	return false;
}

NVECTOR3 IMeshSlicer::mFunction_Compute_Normal2D(NVECTOR3 triangleNormal)
{
	//and you want to know why the projection is the normal , refer to the tech doc
	NVECTOR3 outNormal(triangleNormal.x,0, triangleNormal.z);
	D3DXVec3Normalize(&outNormal,&outNormal);
	return outNormal;
}

//.NOISELAYER loader
bool IMeshSlicer::mFunction_ImportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer)
{
	if (!pLineStripBuffer)
	{
		return false;
	}

	//文件输入流
	std::ifstream fileIn(pFilePath, std::ios::binary);

	//文件不存在就return

	if (!fileIn.is_open())
	{
		ERROR_MSG("FileManager : Cannot Open File !!");
		return false;
	}

	//指针移到文件尾
	fileIn.seekg(0, std::ios_base::end);

	//指针指着文件尾，当前位置就是大小
	int static_fileSize = (int)fileIn.tellg();


	//指针移到文件头
	fileIn.seekg(0, std::ios_base::beg);


	//some  check before importing file
	if (!pLineStripBuffer)
	{
		return false;
	}


	//first import the count data of line strip
	UINT magicNum = 0;
	UINT versionID = 0;
	UINT lineStripCount = 0;
	UINT currLineStripPointCount = 0;
	UINT currLIneStripNormalCount = 0;
	UINT layerID = 0;
	NVECTOR3 tmpV;
	N_LineStrip  emptyLineStrip;
	UINT i = 0, j = 0;


#define STREAM_READ(STREAM,OBJECT) STREAM.read((char*)&(OBJECT),sizeof(OBJECT));

	//file head
	STREAM_READ(fileIn, magicNum);

	STREAM_READ(fileIn, versionID);
	//.........how many line strips
	STREAM_READ(fileIn, lineStripCount);


	//start to read line strip
	for (i = 0;i < lineStripCount;i++)
	{
		//we can push an empty line strip at the back , but didn't specify a layerID
		//because we no longer need (it's used for optimization)
		pLineStripBuffer->push_back(emptyLineStrip);

		STREAM_READ(fileIn, layerID);
		pLineStripBuffer->at(i).LayerID = layerID;

		STREAM_READ(fileIn, currLineStripPointCount);

		STREAM_READ(fileIn, currLIneStripNormalCount);


		//input Points of a line strip
		for (j = 0;j < currLineStripPointCount;j++)
		{
			STREAM_READ(fileIn, tmpV);
			pLineStripBuffer->at(i).pointList.push_back(tmpV);
		}

		//input normals of line segments
		for (j = 0;j < currLIneStripNormalCount;j++)
		{
			STREAM_READ(fileIn, tmpV)
				pLineStripBuffer->at(i).normalList.push_back(tmpV);
		}

	}

	fileIn.close();

	return true;
}

//.NOISELAYER exporter
bool IMeshSlicer::mFunction_ExportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer, bool canOverlapOld)
{
	std::ofstream fileOut;

	//can we overlap the old file??
	if (canOverlapOld)
	{
		fileOut.open(pFilePath, std::ios::binary | std::ios::trunc);
	}
	else
	{
		fileOut.open(pFilePath, std::ios::binary | std::ios::app);
	}

	//check if we have successfully opened the file
	if (!fileOut.good())
	{
		ERROR_MSG("FileManager : Cannot Open File !!");
		return false;
	}

	//prepare to output,tmp var to store number
	UINT i = 0, j = 0;

	/*
	FORMAT:
	4 byte magicNum
	4 byte versionID
	4 byte to store Line Strip Count
	and for every Line Strip :
	first		4 byte for pointList.size()
	then		4 byte for normalList.size(), but it's actually  normalList.size()-1 ,and this is a reminder that
	there are normal data to be read
	then		4 (float) * 3 (vec3 component) *( n + n-1) byte for a whole line strip(vertex + normal)

	keep writing data until all line strip are traversed
	*/

	//convert variables into char* to directly write in a file
#define STREAM_WRITE(STREAM,OBJECT)  {(STREAM).write((char *)&(OBJECT),sizeof(OBJECT));}

	//	first 4 byte for magic number
	char magicNum[] = { 'k','A','s','T' };
	STREAM_WRITE(fileOut, magicNum);

	//	4 byte for version
	UINT32 version = 0xffffff01;
	STREAM_WRITE(fileOut, version);

	//	 4 byte for line strip count
	UINT32 lineStripCount = pLineStripBuffer->size();
	STREAM_WRITE(fileOut, lineStripCount);



	//for every line strip
	for (i = 0;i < pLineStripBuffer->size();i++)
	{
		UINT layerID = pLineStripBuffer->at(i).LayerID;
		STREAM_WRITE(fileOut, layerID);

		//first output points count of current line strip 
		UINT pointListSize = pLineStripBuffer->at(i).pointList.size();
		STREAM_WRITE(fileOut, pointListSize);

		//then normals
		UINT normalListSize = pLineStripBuffer->at(i).normalList.size();
		STREAM_WRITE(fileOut, normalListSize);

		//and traverse every vertices
		for (j = 0;j < pLineStripBuffer->at(i).pointList.size(); j++)
		{
			NVECTOR3 tmpVertex = pLineStripBuffer->at(i).pointList.at(j);
			STREAM_WRITE(fileOut, tmpVertex);
		}

		//and traverse every normal
		for (j = 0;j < pLineStripBuffer->at(i).normalList.size(); j++)
		{
			NVECTOR3 tmpNormal = pLineStripBuffer->at(i).normalList.at(j);
			STREAM_WRITE(fileOut, tmpNormal);
		}
	}

	fileOut.close();

	return true;
}
