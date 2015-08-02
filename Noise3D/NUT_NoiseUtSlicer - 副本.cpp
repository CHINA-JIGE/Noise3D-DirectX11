
/***********************************************************************

							类：NOISE UT SLicer

			简述：用于生成3D打印所需的 切层矢量数据

************************************************************************/
#include "Noise3D.h"


NoiseUtSlicer::NoiseUtSlicer()
{
	m_pPrimitiveVertexBuffer = new std::vector<NVECTOR3>;
	m_pTriangleNormalBuffer = new std::vector<NVECTOR3>	;
	m_pLineSegmentBuffer = new std::vector<N_LineSegment>;
	m_pLineStripBuffer = new std::vector<N_LineStrip>;
	m_pBoundingBox_Min = new NVECTOR3(0,0,0);
	m_pBoundingBox_Max = new NVECTOR3(0, 0, 0);
}


BOOL NoiseUtSlicer::Step1_LoadPrimitiveMeshFromMemory(std::vector<N_DefaultVertex>* pVertexBuffer)
{
	UINT i = 0;

	if (pVertexBuffer->size() == 0)
	{
		return FALSE;
	}

	for (i = 0;i < pVertexBuffer->size();i++)
	{
		m_pPrimitiveVertexBuffer->push_back(pVertexBuffer->at(i).Pos);
	}
	
	//do it right after loading the file
	mFunction_ComputeBoundingBox();

	return TRUE;
}


BOOL NoiseUtSlicer::Step1_LoadPrimitiveMeshFromSTLFile(char * pFilePath)
{
	// this function is used to load STL file , and Primitive Vertex Data has 3x more elements
	//than Triangle Normal Buffer , this is because 1 triangle consists of 3 vertices.

	std::vector<UINT>	tmpIndexBuffer;
	std::vector<char>		tmpInfoBuffer;
	BOOL isLoadSTLsucceeded = FALSE;

	//we must check if the STL has been loaded successfully
	isLoadSTLsucceeded =NoiseFileManager::ImportFile_STL(
		pFilePath, 
		m_pPrimitiveVertexBuffer, 
		&tmpIndexBuffer,
		m_pTriangleNormalBuffer, 
		&tmpInfoBuffer);

	//assert
	if (!isLoadSTLsucceeded)
	{
		DEBUG_MSG("", "", "NoiseUtSliceGenerator: Load STL file failed!!");
		return FALSE;
	}

	//do it right after loading the file
	mFunction_ComputeBoundingBox();

	return TRUE;
}


void NoiseUtSlicer::Step2_Intersection(UINT iLayerCount)
{

	//Objective of this function is GENERALLY  to generate Line segments ,sometimes directly add triangles

	if (iLayerCount < 2)
	{
		DEBUG_MSG("", "", "NoiseUtSlicer : Layer Count is too little !!");
	}

	//top/bottom y of the bounding box (AABB)
	float Y_min = m_pBoundingBox_Min->y;
	float Y_max = m_pBoundingBox_Max->y;


	//calculate  delta Y between layers , note that the TOP and BOTTOM are taken into consideration
	//thus ,  minus 1
	float  layerDeltaY = (Y_max - Y_min) / (float)(iLayerCount-1);

	//...
	UINT		currentLayerID = 0;
	UINT		currentTriangleID = 0;

	//the Y coord of current layer
	float		currentLayerY = 0;

	//how many vertex of a single triangle are on this layer ,valued 0,1,2,3.
	//if vertex count ==0 , we should see if the layer can proceed further intersection with edges
	//which is stored in the "N_IntersectionResult"
	N_IntersectionResult	tmpResult;

	//Total Triangle  Count (one Normal Vector correspond to one Triangle
	UINT		totalTriangleCount = m_pTriangleNormalBuffer->size();


	//........tmp var to store 3 vertex of triangle
	NVECTOR3 v1 =NVECTOR3(0,0,0);		NVECTOR3 v2 = NVECTOR3(0,0,0);	NVECTOR3 v3 = NVECTOR3(0,0,0);

	//.....
	N_LineSegment tmpLineSegment;

	//...tmp var  : used in  "switch (tmpResult.mVertexCount)"
	std::vector<NVECTOR3> tmpIntersectPointList;
	BOOL canIntersect = FALSE;
	NVECTOR3 tmpPoint(0, 0, 0);



	//start traverse all layers / triangles , and intersect
	for (currentLayerID = 0; currentLayerID < iLayerCount;currentLayerID++)
	{
		currentLayerY = Y_min + layerDeltaY * ((float)currentLayerID);


		//calculate how many vertex of this triangle are on this layer
		for (currentTriangleID = 0;currentTriangleID <totalTriangleCount;  currentTriangleID++)
		{
			 v1 = m_pPrimitiveVertexBuffer->at(currentTriangleID * 3 + 0);
			 v2 = m_pPrimitiveVertexBuffer->at(currentTriangleID * 3 + 1);
			 v3 = m_pPrimitiveVertexBuffer->at(currentTriangleID * 3 + 2);

			// tmpResult:"N_IntersectionResult" 
			tmpResult = mFunction_HowManyVertexOnThisLayer(currentTriangleID,currentLayerY,v1,v2,v3);

			//Category Discussion	(?分类讨论?)
			switch (tmpResult.mVertexCount)
			{
			//-------------------------------
			case 0:
#pragma region VertexOnLayer : 0

				if (tmpResult.isPossibleToIntersectEdges)
				{
					//maybe some edges will intersect current Layer
					BOOL canIntersect = FALSE;
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
						N_LineSegment tmpLineSegment;
						tmpLineSegment.v1 = tmpIntersectPointList.at(0);
						tmpLineSegment.v2 = tmpIntersectPointList.at(1);
						tmpLineSegment.LayerID = currentLayerID;
						tmpLineSegment.Dirty = FALSE;
						m_pLineSegmentBuffer->push_back(tmpLineSegment);//add to disordered line segments buffer
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
				switch (tmpResult.mIndexList->at(0))
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
					N_LineSegment tmpLineSegment;
					tmpLineSegment.v1 = tmpIntersectPointList.at(0);
					tmpLineSegment.v2 = tmpIntersectPointList.at(1);
					tmpLineSegment.LayerID = currentLayerID;
					tmpLineSegment.Dirty = FALSE;
					m_pLineSegmentBuffer->push_back(tmpLineSegment);//add to disordered line segments buffer
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
				switch (tmpResult.mIndexList->at(0))
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
				switch (tmpResult.mIndexList->at(1))
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
					N_LineSegment tmpLineSegment;
					tmpLineSegment.v1 = tmpIntersectPointList.at(0);
					tmpLineSegment.v2 = tmpIntersectPointList.at(1);
					tmpLineSegment.LayerID = currentLayerID;
					tmpLineSegment.Dirty = FALSE;
					m_pLineSegmentBuffer->push_back(tmpLineSegment);//add to disordered line segments buffer
				}
				else
				{
					DEBUG_MSG(".", ".", "case 2 : intersect point not equal to 2 !!!");
				}

				// clear the tmpIntersectPoint Buffer
				//tmpIntersectPointList.erase(tmpIntersectPointList.begin(), tmpIntersectPointList.end());
				tmpIntersectPointList.clear();

#pragma endregion 
				break;

			case 3:
#pragma region VertexOnLayer : 3
				//bloody hell....the whole triangle is on this layer..directly  add to line strip buffer
				//and note that the head should be the same point with the tail
				N_LineStrip tmpLineStrip;
				tmpLineStrip.LayerID = currentLayerID;
				tmpLineStrip.pointList.push_back(v1);
				tmpLineStrip.pointList.push_back(v2);
				tmpLineStrip.pointList.push_back(v3);
				tmpLineStrip.pointList.push_back(v1);

				//line strip buffer
				m_pLineStripBuffer->push_back(tmpLineStrip);

#pragma endregion
				break;

			}
		}

	}

}


void	NoiseUtSlicer::Step3_GenerateLineStrip() 
{
	//link numerous line segments into line strip
	//(and at the present  ignore the problem of  'multiple branches'

	N_LineStrip			tmpLineStrip;
	N_LineSegment		tmpLineSegment;
	NVECTOR3			tmpLineStripTailPoint;
	BOOL	canCreateNewLineStrip = TRUE;

	//qualified line segment will link to current line strip , and this UINT determine the line strip;


	//........used to judge if two point can be weld together
	float tmpPointDist = 0;
	const float SAME_POINT_DIST_THRESHOLD = 0.1f;
	NVECTOR3 tmpV;

	//............
	UINT i = 0, j = 0;




	for (i = 0;i < m_pLineSegmentBuffer->size(); i++)
	{
		//if it's time to Create a new branch of line strip, then do it
		if (canCreateNewLineStrip==TRUE)
		{
			//find the first line segment valid to be the head of line strip
			while ( (i < m_pLineSegmentBuffer->size()-1 ) && (tmpLineSegment.Dirty == TRUE))
			{
				i++;
				tmpLineSegment = m_pLineSegmentBuffer->at(i);
			//i++;
			}

			/*if (tmpLineSegment.Dirty == FALSE)
			{
				tmpLineSegment = m_pLineSegmentBuffer->at(i);
			}
			else
			{
				goto nextLineSegment;//located at the end of the "for"
			}*/

			//we have found a "clean" line segment , then add 2 vertices to the current line strip
			//v2 is the tail of the strip
			tmpLineStrip.LayerID = tmpLineSegment.LayerID;
			tmpLineStrip.pointList.push_back(tmpLineSegment.v1);
			tmpLineStrip.pointList.push_back(tmpLineSegment.v2);
			tmpLineStripTailPoint = tmpLineSegment.v2;

			canCreateNewLineStrip = FALSE;
		}


		//now it's time to deal with common situation ,
		//which means we  should add new point to the tail of current "tmpLineStrip"
		
		//and now , traverse all unchecked line segment , and try to link new line segment to it
		for (j = 0;	j < m_pLineSegmentBuffer->size();j++)
		{
			tmpLineSegment = m_pLineSegmentBuffer->at( j );

			//if this line segment has not been checked &&
			//the line segment is on the same layer as the stretching line strip
			if ((tmpLineSegment.Dirty == FALSE) && (tmpLineSegment.LayerID == tmpLineStrip.LayerID))
			{

				//if we can weld v1 and line strip tail 
				tmpV = tmpLineStripTailPoint - tmpLineSegment.v1;
				if(D3DXVec3Length(&tmpV) < SAME_POINT_DIST_THRESHOLD )
				{
					tmpLineStrip.pointList.push_back(tmpLineSegment.v2);
					tmpLineStripTailPoint = tmpLineSegment.v2;
					//this line segment has been checked , so light up the DIRTY mark.
					m_pLineSegmentBuffer->at(j).Dirty = TRUE;

				}
				//else
				
					//if we can weld v2 and line strip tail 
				tmpV = tmpLineStripTailPoint - tmpLineSegment.v2;
				if (D3DXVec3Length(&tmpV) < SAME_POINT_DIST_THRESHOLD)
				{
					tmpLineStrip.pointList.push_back(tmpLineSegment.v1);
					tmpLineStripTailPoint = tmpLineSegment.v1;
					//this line segment has been checked , so light up the DIRTY mark.
					m_pLineSegmentBuffer->at(j).Dirty = TRUE;
			
				}

			}
		
		}

		//we have extended one line strip buffer
		
		m_pLineStripBuffer->push_back(tmpLineStrip);
		canCreateNewLineStrip = TRUE;
		tmpLineStrip.pointList.clear();


	}

}


UINT NoiseUtSlicer::GetLineSegmentCount()
{
	return m_pLineSegmentBuffer->size();
}


void NoiseUtSlicer::GetLineSegmentBuffer(std::vector<NVECTOR3>& outBuffer)
{
	UINT i = 0, j = 0;
	for (i = 0;i < m_pLineSegmentBuffer->size();i++)
	{
		outBuffer.push_back(m_pLineSegmentBuffer->at(i).v1);
		outBuffer.push_back(m_pLineSegmentBuffer->at(i).v2);
	}

	for (i = 0;i < m_pLineStripBuffer->size();i++)
	{
		for (j = 0;j < m_pLineStripBuffer->at(i).pointList.size()-1;j++)
		{
			outBuffer.push_back(m_pLineStripBuffer->at(i).pointList.at(j));
			outBuffer.push_back(m_pLineStripBuffer->at(i).pointList.at(j+1));
		}
	}

}


UINT NoiseUtSlicer::GetLineStripCount()
{
	return m_pLineStripBuffer->size();
}


void NoiseUtSlicer::GetLineStrip(std::vector<NVECTOR3>& outPointList, UINT index)
{
	if (index < m_pLineStripBuffer->size())
	{
		outPointList.assign(	m_pLineStripBuffer->at(index).pointList.begin(),
										m_pLineStripBuffer->at(index).pointList.end());
	}
}



/************************************************************************
											P R I V A T E
************************************************************************/
void	NoiseUtSlicer::mFunction_ComputeBoundingBox()
{
	//compute Bounding box : override 1

	UINT i = 0;
	NVECTOR3 tmpV;
	//traverse all vertices, and get the biggest and smallest point in terms of x,y,z components
	//遍历所有顶点，算出包围盒3分量均最 小/大 的两个顶点
	for (i = 0;i < m_pPrimitiveVertexBuffer->size();i++)
	{
		tmpV = m_pPrimitiveVertexBuffer->at(i);
		if (tmpV.x <(m_pBoundingBox_Min->x)) { m_pBoundingBox_Min->x = tmpV.x; }
		if (tmpV.y <(m_pBoundingBox_Min->y)) { m_pBoundingBox_Min->y = tmpV.y; }
		if (tmpV.z <(m_pBoundingBox_Min->z)) { m_pBoundingBox_Min->z = tmpV.z; }

		if (tmpV.x >(m_pBoundingBox_Max->x)) { m_pBoundingBox_Max->x = tmpV.x; }
		if (tmpV.y >(m_pBoundingBox_Max->y)) { m_pBoundingBox_Max->y = tmpV.y; }
		if (tmpV.z >(m_pBoundingBox_Max->z)) { m_pBoundingBox_Max->z = tmpV.z; }
	}

}


BOOL NoiseUtSlicer::mFunction_Intersect_LineSeg_Layer(NVECTOR3 v1, NVECTOR3 v2, float layerY, NVECTOR3 * outIntersectPoint)
{

	//some obvious wrong input check
	if (!outIntersectPoint){return FALSE;}
	if (v1 == v2) { return FALSE; }

	//init the out result
	ZeroMemory(outIntersectPoint, sizeof(*outIntersectPoint));


	//...detect if it is possible to intersect , according to "Zero Point Existance Theorem(?)"
	if ((v1.y > layerY && v2.y > layerY) || (v1.y < layerY && v2.y < layerY))
	{
		//impossible to intersect ,so just return
		return FALSE;
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
		return TRUE;
	}

	return FALSE;
}


N_IntersectionResult	NoiseUtSlicer::mFunction_HowManyVertexOnThisLayer(UINT iTriangleID, float currentlayerY, NVECTOR3& v1, NVECTOR3& v2, NVECTOR3& v3)

{
	N_IntersectionResult outResult;

	//if all the vertex are beyond / below the layer
	BOOL b1 = (v1.y > currentlayerY) && (v2.y > currentlayerY) && (v3.y > currentlayerY);
	BOOL b2 = (v1.y < currentlayerY) && (v2.y < currentlayerY) && (v3.y < currentlayerY);
	if (b1 || b2 )
	{
		outResult.mVertexCount = 0;
		outResult.isPossibleToIntersectEdges = FALSE;
		return outResult;
	}

	//if none of the vertex is on this layer
	if ((v1.y != currentlayerY) && (v2.y != currentlayerY) && (v3.y != currentlayerY) )
	{
		outResult.mVertexCount = 0;
		outResult.isPossibleToIntersectEdges = TRUE;
		return outResult;
	}


	//note that error exist in float number , 0.001 is a threshold
	const float FLOAT_EQUAL_THRESHOLD = 0.001f;

	//count how many Vertices are on this layer
	if(abs(v1.y - currentlayerY) < FLOAT_EQUAL_THRESHOLD)
	{
		outResult.mVertexCount += 1;
		outResult.mIndexList->push_back(0);
	}

	if (abs(v2.y - currentlayerY) < FLOAT_EQUAL_THRESHOLD)
	{
		outResult.mVertexCount += 1;
		outResult.mIndexList->push_back(1);
	}

	if (abs(v3.y - currentlayerY) < FLOAT_EQUAL_THRESHOLD)
	{
		outResult.mVertexCount += 1;
		outResult.mIndexList->push_back(2);
	}

	return outResult;
}

