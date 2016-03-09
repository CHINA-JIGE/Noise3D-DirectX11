/***********************************************************************

										h£ºFile Manager

************************************************************************/

#pragma once

extern const UINT const_LayerTileStepCount;

struct N_LineStrip
{
	N_LineStrip() { ZeroMemory(this, sizeof(*this)); }//pointList = new std::vector<NVECTOR3>; }

	std::vector<NVECTOR3>	pointList;
	std::vector<NVECTOR3>	normalList;
	UINT		LayerID;
};

struct N_LineSegment
{
	N_LineSegment() { ZeroMemory(this, sizeof(*this)); }
	NVECTOR3 v1;
	NVECTOR3 v2;
	UINT		LayerID;
	NVECTOR3 normal;
	BOOL	Dirty;//check if this line segment has been reviewed,this flag can be reused
};

struct N_IntersectionResult
{
	N_IntersectionResult() { ZeroMemory(this, sizeof(*this));mIndexList = new std::vector<UINT>; }

	UINT mVertexCount;
	BOOL isPossibleToIntersectEdges;//this bool will be used when (mVertexCount ==0)
	std::vector<UINT>* mIndexList;//which vertex (of a triangle) is on the layer
};

struct N_LineSegmentVertex
{
	UINT lineSegmentID;
	UINT vertexID;//v1==1 or v2==2
};

typedef std::vector<N_LineSegmentVertex> N_LayerTile;

struct N_Layer
{
	//2D array, with element count of const_LayerTileStepCount^2
	//layerTile[x][z]
	N_Layer()
	{
		//create a new block with size of (const_LayerTileStepCount x const_LayerTileStepCount)
		layerTile.resize(const_LayerTileStepCount);
		for (UINT i = 0;i < const_LayerTileStepCount;i++)
		{
			layerTile.at(i).resize(const_LayerTileStepCount);
		}
	};

	std::vector<std::vector<N_LayerTile>> layerTile;
};



class _declspec(dllexport) NoiseUtSlicer : private NoiseFileManager
{

public:

	NoiseUtSlicer();

	void		SelfDestruction();

	BOOL	Step1_LoadPrimitiveMeshFromMemory(std::vector<N_DefaultVertex>* pVertexBuffer);

	BOOL	Step1_LoadPrimitiveMeshFromSTLFile(NFilePath pFilePath);

	void		Step2_Intersection(UINT iLayerCount);

	void		Step3_GenerateLineStrip();

	BOOL	Step3_LoadLineStripsFrom_NOISELAYER_File(char* filePath);

	BOOL	Step4_SaveLayerDataToFile(NFilePath filePath);

	UINT		GetLineSegmentCount();

	void		GetLineSegmentBuffer(std::vector<NVECTOR3>& outBuffer);

	UINT		GetLineStripCount();

	void		GetLineStrip(std::vector<N_LineStrip>& outPointList, UINT index);


private:

	void		mFunction_ComputeBoundingBox();
	
	BOOL	mFunction_Intersect_LineSeg_Layer(NVECTOR3 v1, NVECTOR3 v2, float layerY,NVECTOR3* outIntersectPoint);

	void		mFunction_GenerateLayerTileInformation();

	void		mFunction_GetLayerTileIDFromPoint(NVECTOR3 v, UINT& tileID_X, UINT& tileID_Z);

	BOOL	mFunction_LineStrip_FindNextPoint(NVECTOR3* tailPoint, UINT currentLayerID, N_LineStrip* currLineStrip);

	NVECTOR3 mFunction_Compute_Normal2D(NVECTOR3 triangleNormal);
	
	N_IntersectionResult	mFunction_HowManyVertexOnThisLayer(float currentlayerY,NVECTOR3& v1,NVECTOR3& v2,NVECTOR3& v3);

	BOOL mFunction_ImportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer);

	BOOL mFunction_ExportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer, BOOL canOverlapOld);


	std::vector<NVECTOR3>*			m_pPrimitiveVertexBuffer;

	std::vector<NVECTOR3>*			m_pTriangleNormalBuffer;

	std::vector<N_LineSegment>*	m_pLineSegmentBuffer;

	std::vector<N_Layer>*				m_pLayerList;	//for every N_Layer , there are an 2D layer tile array

	std::vector<N_LineStrip>*			m_pLineStripBuffer;

	NVECTOR3*								m_pBoundingBox_Min;

	NVECTOR3*								m_pBoundingBox_Max;
			
};

