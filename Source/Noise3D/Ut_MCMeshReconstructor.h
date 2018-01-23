/******************************************************************


							h : MCMeshReconstructor

			Desc: an mesh(triangle primitive) reconstructor 
				based onMarching Cube algorithm

*******************************************************************/

#pragma once

namespace Noise3D
{
	namespace Ut
	{
		class /*_declspec(dllexport)*/ IMarchingCubeMeshReconstructor
		{
		public:
		
			IMarchingCubeMeshReconstructor();

			//voxel model will be RE-SAMPLED !!! that's why resolution x,y,z are needed.
			bool Compute(const IVoxelizedModel& model, uint16_t resolutionX, uint16_t resolutionY, uint16_t resolutionZ);

			//result are composed of triangles indicated by every 3 vertices 
			//(which means vertex-welding is necessary to generate a visually-smooth model)
			void	GetResult(std::vector<NVECTOR3>& outVertexList);

		private:

			//a cube which could generate new triangles (not valued 0 or 255)
			struct N_NonTrivialCube
			{
				N_NonTrivialCube():
				cubeIndexX(0),cubeIndexY(0),cubeIndexZ(0),triangleCaseIndex(0),arrayEdgeLerpRatio{0.0f}
				{}

				uint16_t	cubeIndexX;
				uint16_t	cubeIndexY;
				uint16_t	cubeIndexZ;
				uint8_t		triangleCaseIndex;
				float			arrayEdgeLerpRatio[12];
			};

			//i,j,k are cube index
			float mFunction_Sample(float i, float j, float k);

			void mFunction_ComputeNonTrivialCase(uint16_t resolutionX, uint16_t resolutionY, uint16_t resolutionZ);

			float mFunction_ComputeEdgeLerpRatio(int edgeID,float start_i,float start_j,float start_k, float stepX, float stepY, float stepZ);

			void mFunction_MarchingCubeGenTriangles(const N_NonTrivialCube& cube);

			const IVoxelizedModel*				m_pVoxelizedModel;

			std::vector<NVECTOR3>				mVertexList;

			float				mResampleScaleX;

			float				mResampleScaleY;

			float				mResampleScaleZ;

			float				mResampledCubeWidth;

			float				mResampledCubeHeight;

			float				mResampledCubeDepth;

			//256¸öTriangle cases£¬8 vertices for one box, 2^8 state combination
			struct N_MCTriangleCase
			{
				char index[16];
			};

			static const float c_SampleBinarizationThreshold;

			static const int	 c_edgeList[12][2];

			static const N_MCTriangleCase c_MarchingCubeTriangleCase[256];
		};
	}
}