/***********************************************************************

										h£ºFile Manager

************************************************************************/

#pragma once


struct N_LineSegment
{
	N_LineSegment() { ZeroMemory(this, sizeof(*this)); }
	NVECTOR3 v1;
	NVECTOR3 v2;
	UINT		LayerID;
	BOOL	Dirty;//check if this line segment has been reviewed,this flag can be reused
};

struct N_IntersectionResult
{
	N_IntersectionResult() { ZeroMemory(this, sizeof(*this));mIndexList = new std::vector<UINT>; }

	UINT mVertexCount;
	BOOL isPossibleToIntersectEdges;//this bool will be used when (mVertexCount ==0)
	std::vector<UINT>* mIndexList;//which vertex (of a triangle) is on the layer
};


public class _declspec(dllexport) NoiseUtSlicer : private NoiseFileManager
{

public:

	NoiseUtSlicer();

	BOOL	Step1_LoadPrimitiveMeshFromMemory(std::vector<N_DefaultVertex>* pVertexBuffer);

	BOOL	Step1_LoadPrimitiveMeshFromSTLFile(char* pFilePath);

	void		Step2_Intersection(UINT iLayerCount);

	void		Step3_GenerateLineStrip();

	UINT		GetLineSegmentCount();

	void		GetLineSegmentBuffer(std::vector<NVECTOR3>& outBuffer);

	UINT		GetLineStripCount();

	void		GetLineStrip(std::vector<NVECTOR3>& outPointList, UINT index);

private:

	void		mFunction_ComputeBoundingBox();
	
	BOOL	mFunction_Intersect_LineSeg_Layer(NVECTOR3 v1, NVECTOR3 v2, float layerY,NVECTOR3* outIntersectPoint);

	N_IntersectionResult	mFunction_HowManyVertexOnThisLayer(UINT iTriangleID, float currentlayerY,NVECTOR3& v1,NVECTOR3& v2,NVECTOR3& v3);

	BOOL	mFunction_LineStrip_FindNextPoint(NVECTOR3* tailPoint,UINT currentLayerID,N_LineStrip* currLineStrip);

	std::vector<NVECTOR3>*			m_pPrimitiveVertexBuffer;

	std::vector<NVECTOR3>*			m_pTriangleNormalBuffer;

	std::vector<N_LineSegment>*	m_pLineSegmentBuffer;

	std::vector<N_LineStrip>*			m_pLineStripBuffer;

	NVECTOR3*								m_pBoundingBox_Min;

	NVECTOR3*								m_pBoundingBox_Max;
			
};

