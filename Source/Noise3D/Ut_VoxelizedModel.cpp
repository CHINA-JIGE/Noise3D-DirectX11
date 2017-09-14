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

float IVoxelizedModel::GetModelWidth() const
{
	return GetVoxelCountX() * GetVoxelWidth();
}

float IVoxelizedModel::GetModelHeight() const
{
	return GetVoxelCountY() * GetVoxelHeight();
}

float IVoxelizedModel::GetModelDepth() const
{
	return GetVoxelCountZ() * GetVoxelDepth();
}

byte IVoxelizedModel::GetVoxel(int x, int y, int z)const
{
	if (x < 0 || y < 0 || z < 0)return 0;

	//voxel bits are all packed into uint32_t 
	if (x < mCubeCountX && y < mCubeCountY&& z < mCubeCountZ)
	{
		uint32_t bitIndex = (y * mCubeCountZ + z) * mCubeCountX + x;//index of bit, y * mCubeCountX * mCubeCountZ + z * mCubeCountX + x;
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

void IVoxelizedModel::SetVoxel(int b, UINT x, UINT y, UINT z)
{
	//voxel bits are all packed into uint32_t 
	/*if (x < mCubeCountX && y < mCubeCountY&& z < mCubeCountZ)
	{
		int val = b ? 1 : 0;
		uint32_t bitIndex =(y * mCubeCountZ + z) * mCubeCountX +x ;//index of bit, y * mCubeCountX * mCubeCountZ + z * mCubeCountX + x;
		uint32_t packedIndex = bitIndex / 32;//index of uint32_t
		uint32_t bitOffset = bitIndex - 32 * packedIndex;
		mVoxelArray.at(packedIndex) &=  ~(1 << bitOffset);//clear bit
		mVoxelArray.at(packedIndex) |=  (val << bitOffset);
	}
	else
	{
		throw std::exception("VoxelizedModel: SetVoxel failure.index out of boundary");
		//ERROR_MSG("VoxelizedModel: SetVoxel failure. index out of boundary");
	}*/
	int val = b!=0 ? 1 : 0;
	uint32_t bitIndex = (y * mCubeCountZ + z) * mCubeCountX + x;//index of bit, y * mCubeCountX * mCubeCountZ + z * mCubeCountX + x;
	uint32_t packedIndex = bitIndex / 32;//index of uint32_t
	uint32_t bitOffset = bitIndex - 32 * packedIndex;
	mVoxelArray.at(packedIndex) &= ~(1 << bitOffset);//clear bit
	mVoxelArray.at(packedIndex) |= (val << bitOffset);
}

void IVoxelizedModel::SetVoxel(int b, UINT startX, UINT endX, UINT y, UINT z)
{
	int val = b != 0 ? 1 : 0;
	uint32_t startBitIndex = (y * mCubeCountZ + z) * mCubeCountX + startX;//index of bit, y * mCubeCountX * mCubeCountZ + z * mCubeCountX + x;
	uint32_t startPackedIndex = startBitIndex / 32;//index of uint32_t
	uint32_t startBitInternalOffset = startBitIndex - 32 * startPackedIndex;
	uint32_t endBitIndex = (y * mCubeCountZ + z) * mCubeCountX + endX;//index of bit, y * mCubeCountX * mCubeCountZ + z * mCubeCountX + x;
	uint32_t endPackedIndex = endBitIndex / 32;//index of uint32_t
	uint32_t endBitInternalOffset = endBitIndex - 32 * endPackedIndex;

	//same packed int
	if (startPackedIndex == endPackedIndex)
	{
		for (uint32_t i = startBitInternalOffset; i <= endBitInternalOffset; ++i)
		{
			mVoxelArray.at(startPackedIndex) &= ~(1 << i);//clear bit
			mVoxelArray.at(startPackedIndex) |= (val << i);
		}
		return;
	}

	//optimization for setting an array of voxel.
	//32 bits can be set in a single operation after optimization
	//|xxxx0000|00000000|00000000|00xxxxxxxx|

	for (uint32_t i = startBitInternalOffset; i < 32; ++i)
	{
		mVoxelArray.at(startPackedIndex) &= ~(1 << i);//clear bit
		mVoxelArray.at(startPackedIndex) |= (val << i);
	}

	//set 32 bit once at a time
	if (val == 0)
	{
		for (uint32_t j = startPackedIndex + 1; j < endPackedIndex; ++j)
		{
			mVoxelArray.at(j) = 0;//every 32 bit int are set to 1
		}
	}
	else
	{
		for (uint32_t j = startPackedIndex + 1; j < endPackedIndex; ++j)
		{
			mVoxelArray.at(j) = 0xffffffff;//every 32 bit int are set to 1
		}
	}

	for (uint32_t k = 0; k <= endBitInternalOffset; ++k)
	{
		mVoxelArray.at(endPackedIndex) &= ~(1 << k);//clear bit
		mVoxelArray.at(endPackedIndex) |= (val << k);
	}

}

bool IVoxelizedModel::SaveToFile_STL(NFilePath STL_filePath)
{
	if (GetVoxelCount() > 10000000)
	{
		ERROR_MSG("VoxelizedModel: SaveToFile_STL failure. Resolution too high.");
		return false;
	}

	std::ofstream outFile(STL_filePath, std::ios::trunc);
	if (!outFile.is_open())
	{
		ERROR_MSG("VoxelizedModel: SaveToFile_STL failure. failed to open file");
		return false;
	}

	std::vector<NVECTOR3> vertexList;

	/*
	     7 ________6
	      /|		   /|
	3  /_____ 2 /  |
	   |	   |4       |  / 5
	   |/_______ |/
	0				1
	
	*/
	float w = 1.0f, h =1.0f, d = 1.0f;
	NVECTOR3 v[8]=
	{
		{0,0,0},
		{w,0,0},
		{w,h,0},
		{0,h,0},
		{0,0,d},
		{w,0,d},
		{w,h,d},
		{0,h,d}
	};

	UINT index[6][6]=
	{
		{0,7,4,0,3,7},//x-minus surface
		{ 5,2,1,5,6,2 },//x-plus surface
		{ 1,0,4,1,4,5 },//y-minus surface
		{ 2,7,3,2,6,7 },//y-plus surface
		{ 1,3,0,1,2,3 },//z-minus surface
		{ 7,6,5,7,5,4 },//z-plus surface
	};

	for (int y = 0; y < mCubeCountY; ++y)
	{
		for (int z = 0; z < mCubeCountZ; ++z)
		{
			for (int x = 0; x < mCubeCountX; ++x)
			{
				byte val = IVoxelizedModel::GetVoxel(x, y, z);
				byte val_xm = IVoxelizedModel::GetVoxel(x-1, y, z);//x-minus
				byte val_xp = IVoxelizedModel::GetVoxel(x+1, y, z);//x-plus
				byte val_ym = IVoxelizedModel::GetVoxel(x, y-1, z);//y-minus
				byte val_yp = IVoxelizedModel::GetVoxel(x, y+1, z);//y-plus
				byte val_zm = IVoxelizedModel::GetVoxel(x, y, z-1);//z-minus
				byte val_zp = IVoxelizedModel::GetVoxel(x, y, z+1);//z-plus
				if (val == 1)
				{
					//cube position
					NVECTOR3 vOffset = NVECTOR3( x*w,y*h,z*d);

					//generate square surface for 1-valued voxel
					if (val_xm == 0)
						for (int i = 0; i < 6; ++i)vertexList.push_back(vOffset+v[index[0][i]]);

					if (val_xp == 0)
						for (int i = 0; i < 6; ++i)vertexList.push_back(vOffset + v[index[1][i]]);

					if (val_ym == 0)
						for (int i = 0; i < 6; ++i)vertexList.push_back(vOffset + v[index[2][i]]);

					if (val_yp == 0)
						for (int i = 0; i < 6; ++i)vertexList.push_back(vOffset + v[index[3][i]]);

					if (val_zm == 0)
						for (int i = 0; i < 6; ++i)vertexList.push_back(vOffset + v[index[4][i]]);

					if (val_zp == 0)
						for (int i = 0; i < 6; ++i)vertexList.push_back(vOffset + v[index[5][i]]);

				}
			}
		}
	}

	return IFileIO::ExportFile_STL_Binary(STL_filePath,"Noise Voxelized Model",vertexList);
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
			for (uint32_t x = 0; x < mCubeCountX; ++x)
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
