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
			N_LayeredLineSegment2D():layerID(0){ }

			Vec2 v1;
			Vec2 v2;
			UINT	layerID;
		};

		struct N_LineStrip
		{
			N_LineStrip():layerID(0) {}//pointList = new std::vector<Vec3>; }

			std::vector<Vec3>	pointList;
			std::vector<Vec3>	normalList;
			UINT		layerID;
		};

		//IFileIO is used to load .stl model
		class /*_declspec(dllexport)*/ MeshSlicer : private IFileIO
		{

		public:

			MeshSlicer();

			bool		Step1_LoadPrimitiveMeshFromMemory(const std::vector<Vec3>& vertexBuffer,const std::vector<UINT>& indexBuffer);

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

			N_AABB	GetBoundingBox();

		private:

			static const UINT c_LayerTileStepCount = 20;

			struct N_LayeredLineSegment
			{
				N_LayeredLineSegment():layerID(0),Dirty(false) {}
				Vec3 v1;
				Vec3 v2;
				UINT		layerID;
				Vec3 normal;
				bool	Dirty;//check if this line segment has been reviewed,this flag can be reused
			};

			struct N_IntersectionResult
			{
				N_IntersectionResult():
					vertexCount(0),
					isPossibleToIntersectEdges(false)
				{ }

				UINT vertexCount;
				bool isPossibleToIntersectEdges;//this bool will be used when (vertexCount ==0)
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

			bool		mFunction_Intersect_LineSeg_Layer(Vec3 v1, Vec3 v2, float layerY, Vec3* outIntersectPoint);

			void		mFunction_GenerateLayerTileInformation();

			void		mFunction_GetLayerTileIDFromPoint(Vec3 v, UINT& tileID_X, UINT& tileID_Z);

			bool		mFunction_LineStrip_FindNextPoint(Vec3* tailPoint, UINT currentLayerID, N_LineStrip* currLineStrip);

			Vec3 mFunction_Compute_Normal2D(Vec3 triangleNormal);

			N_IntersectionResult	mFunction_HowManyVertexOnThisLayer(float currentlayerY, Vec3& v1, Vec3& v2, Vec3& v3);

			bool		mFunction_ImportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer);

			bool		mFunction_ExportFile_NOISELAYER(NFilePath pFilePath, std::vector<N_LineStrip>* pLineStripBuffer, bool canOverlapOld);


			std::vector<Vec3>			mPrimitiveVertexBuffer;

			std::vector<Vec3>			mTriangleNormalBuffer;

			std::vector<N_LayeredLineSegment>	mLineSegmentBuffer;

			std::vector<N_Layer>				mLayerList;	//for every N_Layer , there are an 2D layer tile array

			std::vector<N_LineStrip>		mLineStripBuffer;

			Vec3								mBoundingBox_Min;

			Vec3								mBoundingBox_Max;

			int		mCurrentStep;
		};
	}
}
