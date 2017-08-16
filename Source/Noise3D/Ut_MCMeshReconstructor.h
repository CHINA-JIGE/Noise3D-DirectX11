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
		
			bool Compute(const IVoxelizedModel& model, uint16_t resolutionX, uint16_t resolutionY, uint16_t resolutionZ);

			void	GetResult(std::vector<NVECTOR3>& outVertexList, std::vector<UINT>& outIndexList);

		private:

			//a cube which could generate new triangles (not valued 0 or 255)
			struct N_NonTrivialCube
			{
				uint16_t	cubeIndexX;
				uint16_t	cubeIndexY;
				uint16_t	cubeIndexZ;
				uint8_t		circumstanceIndex;
			};

			void mFunction_MarchingCube();//generate triangle 

			IVoxelizedModel*						m_pVoxelizedModel;
			std::vector<NVECTOR3>				mVertexList;
			std::vector<UINT>						mIndexList;
			std::vector<N_NonTrivialCube>	mNonTrivialCubeList;//non-trivial cube after resampled.
			uint16_t		mResolutionX;
			uint16_t		mResolutionY;
			uint16_t		mResolutionZ;

		};
	}
}