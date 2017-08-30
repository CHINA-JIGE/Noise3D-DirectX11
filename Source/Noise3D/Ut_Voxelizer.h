/***********************************************************************

								h : Voxelizer

				Desc: an Area padder/rasterized that 
				generate BINARIZED pixelsof the inner area 
				of given closing shape 
				(represented as array of line segment)

				Note: IMeshSlicer is a dependency

************************************************************************/

#pragma once
#include "Ut_VoxelizedModel.h"

//2017.8.10 to do: 把体素化的过程集成到voxelize函数里面
//删去binarizedPixelMap，然后体素模型用IVoxelizedModel来表示(一个体素用一个1bit）
//然后voxelizer的体素表示也换成voxelizedModel用来优化空间效率吧

namespace Noise3D
{
	namespace Ut
	{

		class /*_declspec(dllexport)*/ IVoxelizer
		{

		public:

			IVoxelizer();

			bool Init(NFilePath STLModelFile, uint16_t cubeCountX, uint16_t cubeCountY, uint16_t cubeCountZ);

			bool Init(const std::vector<NVECTOR3>& vertexList,const std::vector<UINT>& indexList, UINT cubeCountX, UINT cubeCountY, UINT cubeCountZ, float cubeWidth, float cubeHeight, float cubeDepth);

			void Voxelize();

			void GetVoxelizedModel(IVoxelizedModel& outModel);

		private:

			//use scan line algorithm, every line segments will intersect with
			//each scan line , then there will many intersect points on each scan line
			//because scan line is parallel with x axis, thus only x coordinate need to be stored
			//thus one 'N_IntersectXCoordList' saves all intersected points' x coord in EACH LAYER
			typedef std::vector<std::vector<float>> N_IntersectXCoordList;

			//but if one or two vertices of line segment lies right on the scanline
			//then ambiguous situation happens
			struct N_AmbiguousScanlineCircumstance
			{
				std::vector<N_LayeredLineSegment2D> activeLineSegmentList;
				UINT layerID;
				UINT scanLineRowID;
				float originalScanlineYCoord;
			};

			void mFunction_Rasterize(const std::vector<N_LayeredLineSegment2D>& lineSegList);

			void mFunction_LineSegment_Scanline_Intersect(const N_LayeredLineSegment2D& line, UINT scanlineRowID, float y);

			// optional process after line rasterization(pad the inside area of closed lines)
			void mFunction_PadInnerArea(N_IntersectXCoordList& layer,UINT layerID);


			bool		mIsInitialized;
			//intermediate data storing result of scanline filling algorithm (lineSegment intersection)
			std::vector<N_IntersectXCoordList> mIntersectXCoordLayers;
			//ambiguous scanline intersection result (line segment vertex on the scanline)
			std::vector<N_AmbiguousScanlineCircumstance> mAmbiguousIntersection;

			IMeshSlicer		mSlicer;

			IVoxelizedModel mVoxelizedModel;

			NVECTOR2 mLayerPosMin;

			NVECTOR2 mLayerPosMax;

			float	mLayerRealWidth;//boundingbox' XZ of original mesh

			float mLayerRealDepth;

		};

	}
}