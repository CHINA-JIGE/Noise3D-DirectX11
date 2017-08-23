/***********************************************************************

								h : Voxelized Model

							Desc: Used by Voxelizer

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace Ut
	{

		class IVoxelizedModel
		{
		public:

			IVoxelizedModel();

			IVoxelizedModel(const IVoxelizedModel& model);//deep copy is required

			bool	Resize(uint16_t cubeCountX, uint16_t cubeCountY, uint16_t cubeCountZ,float cubeWidth,float cubeHeight,float cubeDepth);

			float GetVoxelWidth() const;

			float GetVoxelHeight() const;

			float GetVoxelDepth() const;

			UINT GetVoxelCountX() const ;

			UINT GetVoxelCountY()const;

			UINT GetVoxelCountZ()const;

			UINT GetVoxelCount()const;

			//1, those {x,y,z} out of boundary will yield a 0
			//2, {y{z{x}}} nested loop access is more memory coherent
			byte GetVoxel(int x, int y, int z)const;

			void	SetVoxel(bool b, UINT x, UINT y, UINT z);

			bool	SaveToFile_NVM(NFilePath NVM_filePath);//Noise Voxelized Model

			bool	SaveToFile_TXT(NFilePath TXT_filePath);//Noise Voxelized Model

			bool LoadFromFile_NVM(NFilePath NVM_filePath);//Noise Voxelized Model

		private:

			//typedef std::vector<std::vector<byte>> N_LayerBitmap;

			//every bit represent the presence/absence of a voxel
			//std::vector<N_LayerBitmap> mLayerGroup;
			std::vector<uint32_t>	mVoxelArray;

			float			mCubeWidth;//x

			float			mCubeHeight;//y

			float			mCubeDepth;//z

			uint16_t	mCubeCountX;

			uint16_t	mCubeCountY;

			uint16_t	mCubeCountZ;

		};

	}
}
