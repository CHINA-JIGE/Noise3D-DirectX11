
/***********************************************************************

							类：NOISE UT SLice Generator

			简述：用于生成3D打印所需的 切层矢量数据

************************************************************************/
#include "Noise3D.h"


NoiseUtLayerGenerator::NoiseUtLayerGenerator()
{
	m_pPrimitiveVertexBuffer = new std::vector<NVECTOR3>;
	m_pTriangleNormalBuffer = new std::vector<NVECTOR3>	;
	m_pLineSegmentBuffer = new std::vector<N_LineSegment>;
	m_pLineStripBuffer = new std::vector<N_LineStrip>;
	m_pBoundingBox_Min = new NVECTOR3(0,0,0);
	m_pBoundingBox_Max = new NVECTOR3(0, 0, 0);
}

BOOL NoiseUtLayerGenerator::Step1_LoadPrimitiveMeshFromMemory(std::vector<N_DefaultVertex>* pVertexBuffer)
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

BOOL NoiseUtLayerGenerator::Step1_LoadPrimitiveMeshFromSTLFile(char * pFilePath)
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
	assert(isLoadSTLsucceeded: "NoiseUtSliceGenerator: Load STL file failed!!");

	//do it right after loading the file
	mFunction_ComputeBoundingBox();

	return TRUE;
}

void NoiseUtLayerGenerator::Step2_Intersection(int iLayerCount)
{

	//Objective of this function is GENERALLY  to generate Line segments ,sometimes directly add triangles


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
			tmpResult = mFunction_HowManyVertexOnThisLayer(currentTriangleID,currentLayerID,v1,v2,v3);

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
					else
					{
						DEBUG_MSG("","","case 0 : intersect point not equal to 2 !!!");
					}

					// clear the tmpIntersectPoint Buffer
					tmpIntersectPointList.erase(tmpIntersectPointList.begin(), tmpIntersectPointList.end());
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
					DEBUG_MSG("", "", "case 1: intersect point not equal to 2 !!!");
				}

				// clear the tmpIntersectPoint Buffer
				tmpIntersectPointList.erase(tmpIntersectPointList.begin(), tmpIntersectPointList.end());

			#pragma endregion 
				break;

			//-------------------------------
			case 2:
				#pragma region VertexOnLayer : 2
				//v1,v2 are on this layer ,so just directly add to line segment buffer
				if (tmpResult.mIndexList->at(0) == 0)
				{
					tmpIntersectPointList.push_back(v1);
				}
				if (tmpResult.mIndexList->at(1) == 1)
				{
					tmpIntersectPointList.push_back(v2);
				}
				if (tmpResult.mIndexList->at(2) ==2)
				{
					tmpIntersectPointList.push_back(v3);
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
					DEBUG_MSG("", "", "case 2 : intersect point not equal to 2 !!!");
				}

				// clear the tmpIntersectPoint Buffer
				tmpIntersectPointList.erase(tmpIntersectPointList.begin(), tmpIntersectPointList.end());


				#pragma endregion 
				break;

			case 3:
				#pragma region VertexOnLayer : 3
				//bloody hell....the whole triangle is on this layer..directly  add to line strip buffer
				N_LineStrip tmpLineStrip;
				tmpLineStrip.LayerID = currentLayerID;
				tmpLineStrip.pointList->push_back(v1);
				tmpLineStrip.pointList->push_back(v2);
				tmpLineStrip.pointList->push_back(v3);

				//line strip buffer
				m_pLineStripBuffer->push_back(tmpLineStrip);

				#pragma endregion
				break;

			}
		}

	}

}



/************************************************************************
											P R I V A T E
************************************************************************/
void	NoiseUtLayerGenerator::mFunction_ComputeBoundingBox()
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

BOOL NoiseUtLayerGenerator::mFunction_Intersect_LineSeg_Layer(NVECTOR3 v1, NVECTOR3 v2, float layerY, NVECTOR3 * outIntersectPoint)
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
	float lambda = (v1.y - v2.y) / (layerY - v2.y);

	//actually not an essential check,but of no harm = =
	if (lambda > -1.0f or lambda < 1.0f)
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

N_IntersectionResult	NoiseUtLayerGenerator::mFunction_HowManyVertexOnThisLayer(UINT iTriangleID, float currentlayerY, NVECTOR3& v1, NVECTOR3& v2, NVECTOR3& v3)

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

