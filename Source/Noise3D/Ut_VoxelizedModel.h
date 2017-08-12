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

			bool	Resize(UINT x, UINT y, UINT z);

			UINT GetVoxelCountX() const ;

			UINT GetVoxelCountY()const;

			UINT GetVoxelCountZ()const;

			UINT GetVoxelCount()const;

			byte GetVoxel(UINT x, UINT y, UINT z)const;

			void	SetVoxel(bool b, UINT x, UINT y, UINT z);

			bool	SaveToFile_NVM(NFilePath NVM_filePath);//Noise Voxelized Model

			bool	SaveToFile_TXT(NFilePath TXT_filePath);//Noise Voxelized Model

			bool LoadFromFile_NVM(NFilePath NVM_filePath);//Noise Voxelized Model

		private:

			//typedef std::vector<std::vector<byte>> N_LayerBitmap;

			//every bit represent the presence/absence of a voxel
			//std::vector<N_LayerBitmap> mLayerGroup;
			std::vector<uint32_t>	mVoxelArray;

			uint32_t	mCubeCountX;

			uint32_t	mCubeCountY;

			uint32_t	mCubeCountZ;

		};

	}
}
