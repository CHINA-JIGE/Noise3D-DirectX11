/*********************************************************

							cpp: Voxelizer

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

IVoxelizedModel::IVoxelizedModel():
	mCubeCountX(0),
	mCubeCountY(0),
	mCubeCountZ(0)
{
}

IVoxelizedModel::IVoxelizedModel(const IVoxelizedModel & model)
{
	mCubeWidth = model.mCubeWidth;
	mCubeHeight = model.mCubeHeight;
	mCubeDepth = model.mCubeDepth;

	mCubeCountX = model.mCubeCountX;
	mCubeCountY = model.mCubeCountY;
	mCubeCountZ = model.mCubeCountZ;
	mVoxelArray = model.mVoxelArray;
}

bool IVoxelizedModel::Resize(uint16_t cubeCountX, uint16_t cubeCountY, uint16_t cubeCountZ, float cubeWidth, float cubeHeight, float cubeDepth)
{
	//	  Y|        
	//		|      /Z
	//		|	 /
	//		|  /
	//		|/___________X
	//
	//

	//resolution overflow check
	uint32_t tmpOverflowCheck=UINT32_MAX;
	tmpOverflowCheck /= cubeCountX;
	if (cubeCountY > tmpOverflowCheck)
	{
		ERROR_MSG("VoxelizedModel: resize failure. Resolution exceed limit.");
		return false;
	}
	tmpOverflowCheck /= cubeCountY;
	if (cubeCountZ > tmpOverflowCheck)
	{
		ERROR_MSG("VoxelizedModel: resize failure. Resolution exceed limit.");
		return false;
	}

	mCubeWidth = cubeWidth;
	mCubeHeight = cubeHeight;
	mCubeDepth = cubeDepth;
	mCubeCountX = cubeCountX;
	mCubeCountY = cubeCountY;
	mCubeCountZ = cubeCountZ;

	//1 bit for each voxel
	UINT uintCount = (GetVoxelCount() / 32)+1;
	mVoxelArray.resize(uintCount, 0);

	/*mLayerGroup.resize(y);//y layers

	for (auto& layer: mLayerGroup)
	{
		layer.resize(z);

		for (auto& RowX: layer)
		{
			//every row is a continuous X coord series
			RowX.resize((x/8), 0);
		}
	}*/

	return true;
}

float IVoxelizedModel::GetVoxelWidth() const
{
	return mCubeWidth;
}

float IVoxelizedModel::GetVoxelHeight() const
{
	return mCubeHeight;
}

float IVoxelizedModel::GetVoxelDepth() const
{
	return mCubeDepth;
}

UINT IVoxelizedModel::GetVoxelCountX() const
{
	return mCubeCountX;
}

UINT IVoxelizedModel::GetVoxelCountY() const
{
	return mCubeCountY;
}

UINT IVoxelizedModel::GetVoxelCountZ() const
{
	return mCubeCountZ;
}

UINT IVoxelizedModel::GetVoxelCount() const
{
	return mCubeCountX * mCubeCountY * mCubeCountZ;
}

byte IVoxelizedModel::GetVoxel(int x, int y, int z)const
{
	if (x < 0 || y < 0 || z < 0)return 0;

	//voxel bits are all packed into uint32_t 
	if (x < mCubeCountX && y < mCubeCountY&& z < mCubeCountZ)
	{
		uint32_t bitIndex = (y * mCubeCountX + z) * mCubeCountZ + x;//index of bit, y * mCubeCountX * mCubeCountZ + z * mCubeCountX + x;
		uint32_t packedIndex = bitIndex  / 32;//index of uint32_t
		uint32_t bitOffset = bitIndex - 32 * packedIndex;
		byte val = (mVoxelArray.at(packedIndex) & (1 << bitOffset))>>bitOffset;
		return val;
	}
	else
	{
		//WARNING_MSG("VoxelizedModel: GetVoxel failure. index out of boundary");
		return 0;
	}
}

void IVoxelizedModel::SetVoxel(bool b, UINT x, UINT y, UINT z)
{
	//voxel bits are all packed into uint32_t 
	if (x < mCubeCountX && y < mCubeCountY&& z < mCubeCountZ)
	{
		int val = b ? 1 : 0;
		uint32_t bitIndex =(y * mCubeCountX + z) * mCubeCountZ +x ;//index of bit, y * mCubeCountX * mCubeCountZ + z * mCubeCountX + x;
		uint32_t packedIndex = bitIndex / 32;//index of uint32_t
		uint32_t bitOffset = bitIndex - 32 * packedIndex;
		mVoxelArray.at(packedIndex) |= (val << bitOffset);
	}
	else
	{
		ERROR_MSG("VoxelizedModel: SetVoxel failure. index out of boundary");
	}

	return;
}

bool IVoxelizedModel::SaveToFile_NVM(NFilePath NVM_filePath)
{
	std::ofstream outFile(NVM_filePath, std::ios::binary);
	if (!outFile.is_open())
	{
		ERROR_MSG("VoxelizedModel: SaveToFile_NVM failure. failed to open file");
		return false;
	}

	//file structure:
	// cubeCountX - 4bytes
	// cubeCountY - 4bytes
	// cubeCountZ - 4bytes
	// voxels data (packed data)
#define WRITE(var) outFile.write((char*)&var,sizeof(var))

	WRITE(mCubeCountX);
	WRITE(mCubeCountY);
	WRITE(mCubeCountZ);

	outFile.write((char*)&mVoxelArray.at(0), mVoxelArray.size() * sizeof(uint32_t));
	outFile.close();

	return true;
}

bool IVoxelizedModel::SaveToFile_TXT(NFilePath TXT_filePath)
{
	if (GetVoxelCount() > 20000000)
	{
		ERROR_MSG("VoxelizedModel: SaveToFile_TXT failure. Resolution too high, time could be last too long");
		return false;
	}

	std::ofstream outFile(TXT_filePath, std::ios::trunc);
	if (!outFile.is_open())
	{
		ERROR_MSG("VoxelizedModel: SaveToFile_TXT failure. failed to open file");
		return false;
	}

	//file structure:
	// cubeCountX - 4bytes
	// cubeCountY - 4bytes
	// cubeCountZ - 4bytes
	// voxels data (packed data)
	outFile << "cubeCountX:" <<mCubeCountX << std::endl;
	outFile << "cubeCountY:" << mCubeCountY << std::endl;
	outFile << "cubeCountZ:" << mCubeCountZ << std::endl;

	for (uint32_t y = 0; y < mCubeCountY; ++y)
	{
		outFile << "***Layer" << y << std::endl;
		for (uint32_t z = 0; z < mCubeCountZ; ++z)
		{
			for (uint32_t x = 0; x < mCubeCountZ; ++x)
			{
				outFile << int( IVoxelizedModel::GetVoxel(x, y, z));
			}
			outFile << std::endl;
		}
	}

	outFile.close();

	return true;
}

bool IVoxelizedModel::LoadFromFile_NVM(NFilePath NVM_filePath)
{
	std::ifstream inFile(NVM_filePath, std::ios::binary);
	if (!inFile.is_open())
	{
		ERROR_MSG("VoxelizedModel: LoadFromFile_NVM failure. failed to open file");
		return false;
	}

	//file structure:
	// cubeCountX - 4bytes
	// cubeCountY - 4bytes
	// cubeCountZ - 4bytes
	// voxels data (packed data)
#define READ(var) inFile.read((char*)&var,sizeof(var))

	READ(mCubeCountX);
	READ(mCubeCountY);
	READ(mCubeCountZ);

	IVoxelizedModel::Resize(mCubeCountX, mCubeCountY, mCubeCountZ,1.0f,1.0f,1.0f);
	inFile.read((char*)&mVoxelArray.at(0), mVoxelArray.size() * sizeof(uint32_t));
	inFile.close();

	return true;
}
