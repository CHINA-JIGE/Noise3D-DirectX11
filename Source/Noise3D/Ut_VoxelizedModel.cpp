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
	
}

bool IVoxelizedModel::Resize(UINT x, UINT y, UINT z)
{
	//	  Y|        
	//		|      /Z
	//		|	 /
	//		|  /
	//		|/___________X
	//
	//

	//resolution overflow check
	UINT tmpOverflowCheck=UINT32_MAX;
	tmpOverflowCheck /= x;
	if (y > tmpOverflowCheck)
	{
		ERROR_MSG("VoxelizedModel: resize failure. Resolution exceed limit.");
		return false;
	}
	tmpOverflowCheck /= y;
	if (z > tmpOverflowCheck)
	{
		ERROR_MSG("VoxelizedModel: resize failure. Resolution exceed limit.");
		return false;
	}


	mCubeCountX = x;
	mCubeCountY = y;
	mCubeCountZ = z;

	//1 bit for each voxel
	UINT uintCount = (GetVoxelCount() / sizeof(uint32_t))+1;
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

byte IVoxelizedModel::GetVoxel(UINT x, UINT y, UINT z)const
{
	return 0;
}
