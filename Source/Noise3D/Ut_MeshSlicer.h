/***********************************************************************

										h£ºMesh Slicer

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace Ut
	{
		struct N_LayeredLineSegment2D
		{
			N_LayeredLineSegment2D() { ZeroMemory(this, sizeof(*this)); }

			NVECTOR2 v1;
			NVECTOR2 v2;
			UINT	layerID;
		};

		struct N_LineStrip
		{
			N_LineStrip() { ZeroMemory(this, sizeof(*this)); }//pointList = new std::vector<NVECTOR3>; }

			std::vector<NVECTOR3>	pointList;
			std::vector<NVECTOR3>	normalList;
			UINT		LayerID;
		};

		//IFileIO is used to load .stl model
		class /*_declspec(dllexport)*/ IMeshSlicer : private IFileIO
		{

		public:

			IMeshSlicer();

			bool		Step1_LoadPrimitiveMeshFromMemory(const std::vector<NVECTOR3>& vertexBuffer,const std::vector<UINT>& indexBuffer);

			bool		Step1_LoadPrimitiveMeshFromMemory(const std::vector<N_DefaultVertex>& vertexBuffer);

			bool		Step1_LoadPrimitiveMeshFromSTLFile(NFilePath pFilePath);

			void		Step2_Intersection(UINT iLayerCount);

			void		Step3_GenerateLineStrip();

			bool		Step3_LoadLineStripsFrom_NOISELAYER_File(char* filePath);

			bool		Step4_SaveLayerDataToFile(NFilePath filePath);

			UINT	GetLineSegmentCount();

			void		GetLineSegmentBuffer(std::vector<N_LayeredLineSegment2D>& outBuffer);

			UINT	GetLineStripCount();

			void		GetLineStrip(std::vector<N_LineStrip>& outPointList, UINT index);

			N_Box	GetBoundingBox();

		private:

			static const UINT c_LayerTileStepCount = 20;

			struct N_LayeredLineSegment
			{
				N_LayeredLineSegment() { ZeroMemory(this, sizeof(*this)); }
				NVECTOR3 v1;
				NVECTOR3 v2;
				UINT		LayerID;
				NVECTOR3 normal;
				bool	Dirty;//check if this line segment has been reviewed,this flag can be reused
			};

			struct N_IntersectionResult
			{
				N_IntersectionResult() { ZeroMemory(this, sizeof(*this));}

				UINT mVertexCount;
				bool isPossibleToIntersectEdges;//this bool will be used when (mVertexCount ==0)
				std::vector<UINT> mIndexList;//which vertex (of a triangle) is on the layer
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
					layerTile.resize(c_LayerTileStepCount);
					for (UINT i = 0; i < c_LayerTileStepCount; i++)
					{
						layerTile.at(i).resize(c_LayerTileStepCount);
					}
				};

				std::vector<std::vector<N_LayerTile>> layerTile;
			};


			void		mFunction_ComputeBoundingBox();

			bool		mFunction_Intersect_LineSeg_Layer(NVECTOR3 v1, NVECTOR3 v2, float layerY, NVECTOR3* outIntersectPoint);

			void		mFunction_GenerateLayerTileInformation();

			void		mFunction_GetLayerTileIDFromPoint(NVECTOR3 v, UINT& tileID_X, UINT& tileID_Z);

			bool		mFunction_LineStrip_FindNextPoint(NVECTOR3* tailPoint, UINT currentLayerID, N_LineStrip* currLineStrip);

			NVECTOR3 mFunction_Compute_Normal2D(NVECTOR3 triangleNormal);

			N_IntersectionResult	mFunction_HowManyVertexOnThisLayer(float currentlayerY, NVECTOR3& v1, NVECTOR3& v2, NVECTOR3& v3);

			bool		mFunction_ImportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer);

			bool		mFunction_ExportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer, bool canOverlapOld);


			std::vector<NVECTOR3>			mPrimitiveVertexBuffer;

			std::vector<NVECTOR3>			mTriangleNormalBuffer;

			std::vector<N_LayeredLineSegment>	mLineSegmentBuffer;

			std::vector<N_Layer>				mLayerList;	//for every N_Layer , there are an 2D layer tile array

			std::vector<N_LineStrip>		mLineStripBuffer;

			NVECTOR3								mBoundingBox_Min;

			NVECTOR3								mBoundingBox_Max;

			int		mCurrentStep;
		};
	}
}
