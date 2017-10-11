/******************************************************************

								cpp: MCMeshReconstructor

*******************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

const float  IMarchingCubeMeshReconstructor::c_SampleBinarizationThreshold = 0.1f;//wtf???

IMarchingCubeMeshReconstructor::IMarchingCubeMeshReconstructor():
	mResampledCubeWidth(0.0f),
	mResampledCubeHeight(0.0f),
	mResampledCubeDepth(0.0f),
	mResampleScaleX(1.0f),
	mResampleScaleY(1.0f),
	mResampleScaleZ(1.0f),
	m_pVoxelizedModel(nullptr)
{
}

bool IMarchingCubeMeshReconstructor::Compute(const IVoxelizedModel & model, uint16_t resolutionX, uint16_t resolutionY, uint16_t resolutionZ)
{
	mVertexList.clear();
	m_pVoxelizedModel = &model;
	mFunction_ComputeNonTrivialCase(resolutionX, resolutionY, resolutionZ);

	return true;
}

void IMarchingCubeMeshReconstructor::GetResult(std::vector<NVECTOR3>& outVertexList)
{
	outVertexList = mVertexList;
}

/***************************************************

								P R I V A T E

****************************************************/


float IMarchingCubeMeshReconstructor::mFunction_Sample(float  i ,float j, float k)
{
	//desc: tri-linear sample, which takes 8 3d-vertices as source input
	//i,j,k are SCALED  index coordinate

	//NOTE: in order to "close" the MC reconstructed mesh, boundary of the big
	//voxel model must be 0
	//for example:
	/*
	
	00100
	11111
	11111 
	00110

	(for the 2nd and 3rd row, the left and right boundary didn't generate any iso-surface)
	thus we should add 0s to the boundaries so that iso-surface near the boundary can be generated 

	*/

	if (i < 0.0f || j < 0.0f || k < 0.0f)return 0.0f;

	float sampleCoordX = float(i) * mResampleScaleX;
	float sampleCoordY = float(j) * mResampleScaleY;
	float sampleCoordZ = float(k) * mResampleScaleZ;
	int baseIndexX = int(sampleCoordX);
	int baseIndexY = int(sampleCoordY);
	int baseIndexZ = int(sampleCoordZ);

	if (baseIndexX >= int(m_pVoxelizedModel->GetVoxelCountX()) ||
		baseIndexY >= int(m_pVoxelizedModel->GetVoxelCountY()) ||
		baseIndexZ >= int(m_pVoxelizedModel->GetVoxelCountZ()))
	{
		return 0.0f;
	}



	/*

	    6 _________7
	     /			   /|
	4  /____5 _ /  |
	   |	            |  / 3
	   | ______  |/
	0				1

	Y|
	  |    /Z
	  |  /
	  |/___________X

	*/

	//i --- x
	//j --- z
	//k --- y


	byte val[8];
	val[0] = m_pVoxelizedModel->GetVoxel(baseIndexX,			baseIndexY,			baseIndexZ);
	val[1] = m_pVoxelizedModel->GetVoxel(baseIndexX + 1,		baseIndexY,			baseIndexZ);
	val[2] = m_pVoxelizedModel->GetVoxel(baseIndexX,			baseIndexY,			baseIndexZ + 1);
	val[3] = m_pVoxelizedModel->GetVoxel(baseIndexX + 1,		baseIndexY,			baseIndexZ + 1);
	val[4] = m_pVoxelizedModel->GetVoxel(baseIndexX,			baseIndexY + 1,	baseIndexZ);
	val[5] = m_pVoxelizedModel->GetVoxel(baseIndexX + 1,		baseIndexY + 1,	baseIndexZ);
	val[6] = m_pVoxelizedModel->GetVoxel(baseIndexX,			baseIndexY + 1,	baseIndexZ + 1);
	val[7] = m_pVoxelizedModel->GetVoxel(baseIndexX + 1,		baseIndexY + 1,	baseIndexZ + 1);

	int sum = 0;
	for (int vertexID = 0; vertexID < 8; ++vertexID)sum += val[vertexID];
	if (sum == 0)return 0.0f;
	if (sum == 8)return 1.0f;

	// !!!!! Triple Bilinear Interpolation !!!!!!!!!!

	//1, get the fractional part of float-index for LERP ratio
	float u = (sampleCoordX - float(baseIndexX));
	float v = (sampleCoordY - float(baseIndexY));
	float w = (sampleCoordZ - float(baseIndexZ));

	//2, tri-linear interpolation
	float lerpAB = Noise3D::Lerp(val[0], val[1], u);
	float lerpCD = Noise3D::Lerp(val[2], val[3], u);
	float lerpEF = Noise3D::Lerp(val[4], val[5], u);
	float lerpGH = Noise3D::Lerp(val[6], val[7], u);
	float lerpABCD = Noise3D::Lerp(lerpAB, lerpCD, w);
	float lerpEFGH = Noise3D::Lerp(lerpEF, lerpGH, w);
	float resampleValue = Noise3D::Lerp(lerpABCD, lerpEFGH, v);

	return resampleValue;
	//return val[0];
}

void IMarchingCubeMeshReconstructor::mFunction_ComputeNonTrivialCase(uint16_t resolutionX, uint16_t resolutionY, uint16_t resolutionZ)
{
	//compute voxel size after resampled (re-scale)
	mResampleScaleX = float(m_pVoxelizedModel->GetVoxelCountX()) / float(resolutionX);
	mResampleScaleY = float(m_pVoxelizedModel->GetVoxelCountY()) / float(resolutionY);
	mResampleScaleZ = float(m_pVoxelizedModel->GetVoxelCountZ()) / float(resolutionZ);
	//Abbreviation of scales
	float scaleX = mResampleScaleX;
	float scaleY = mResampleScaleY;
	float scaleZ = mResampleScaleZ;
	mResampledCubeWidth = m_pVoxelizedModel->GetVoxelWidth() * mResampleScaleX;
	mResampledCubeHeight = m_pVoxelizedModel->GetVoxelHeight() * mResampleScaleY;
	mResampledCubeDepth = m_pVoxelizedModel->GetVoxelDepth() * mResampleScaleZ;

	//Re-sample and generate Non-trivial cubes
	//note £º in order to preserve memory coherence during access,
	//nested loop will be {y{z{x}}}. for details plz refer to "IVoxelizedModel"
	for (int j = -1; j < resolutionY; ++j)
	{
		for (int k = -1; k < resolutionZ; ++k)
		{
			for (int i = -1; i < resolutionX; ++i)
			{
			/*
			the cube define in Marching Cube[Lorensen 1987]

	     7 ________6
	      /|		   /|
	3  /_____ 2 /  |
	   |	   |4       |  / 5
	   |/_______ |/
	0				1

	Y|
	  |    /Z
	  |  /
	  |/___________X

				*/

				N_NonTrivialCube cube;
				cube.cubeIndexX = i+1;
				cube.cubeIndexY = j+1;
				cube.cubeIndexZ = k+1;


				//in this stage, vertices are re-sampled and have fractional weight (not binary anymore)
				//i,j,k are SCALED index
				float resampledArray[8];
				resampledArray[0] = mFunction_Sample(i+0.0f,	j +0.0f,		k +0.0f);//i,j,k will be scaled
				resampledArray[1] = mFunction_Sample(i+1.0f,	j + 0.0f,	k + 0.0f);
				resampledArray[2] = mFunction_Sample(i+1.0f,	j +1.0f,		k + 0.0f);
				resampledArray[3] = mFunction_Sample(i+0.0f,	j +1.0f,		k + 0.0f);
				resampledArray[4] = mFunction_Sample(i+0.0f,	j + 0.0f,	k +1.0f);
				resampledArray[5] = mFunction_Sample(i+1.0f,	j + 0.0f,	k +1.0f);
				resampledArray[6] = mFunction_Sample(i+1.0f,	j +1.0f,		k +1.0f);
				resampledArray[7] = mFunction_Sample(i+0.0f,	j +1.0f,		k +1.0f);

				//compute triangle case ID (those 256 circumstances)
				int triangleCase=0;
				for (int vertexID = 0; vertexID < 8; ++vertexID)
				{
					//though resampled vertices has non-binary weight,
					//yet the calculation of triangle case index needs BINARY indicator
					if (resampledArray[vertexID] >= c_SampleBinarizationThreshold)
					{
						triangleCase |= (1 << vertexID);
					}
				}
				//skip TRIVIAL cases ( which doesn't generate triangles)
				if (triangleCase == 0 || triangleCase == 255)continue;

				cube.triangleCaseIndex = triangleCase;

				//calculate LERP ratio of point on edges
				/*	{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
				{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
				{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 } };*/

				//find the intersect point of edge and iso-surface
				//yielding lerp ratio for 12 edges of the cube ( a-->b )
				float stepX = 1.0f / scaleX;
				float stepY = 1.0f / scaleY;
				float stepZ = 1.0f / scaleZ;
				cube.arrayEdgeLerpRatio[0] = mFunction_ComputeEdgeLerpRatio(0, i + 0.0f,		j +0.0f,		k + 0.0f,		stepX, .0f, .0f);
				cube.arrayEdgeLerpRatio[1] = mFunction_ComputeEdgeLerpRatio(1, i + 1.0f,		j +0.0f,		k + 0.0f,		.0f, stepY, .0f);
				cube.arrayEdgeLerpRatio[2] = mFunction_ComputeEdgeLerpRatio(2, i + 0.0f,		j +1.0f,		k + 0.0f,		stepX, .0f, .0f);
				cube.arrayEdgeLerpRatio[3] = mFunction_ComputeEdgeLerpRatio(3, i + 0.0f,		j +0.0f,		k + 0.0f,		.0f, stepY, .0f);
				cube.arrayEdgeLerpRatio[4] = mFunction_ComputeEdgeLerpRatio(4, i + 0.0f,		j +0.0f,		k +1.0f,		stepX, .0f, .0f);
				cube.arrayEdgeLerpRatio[5] = mFunction_ComputeEdgeLerpRatio(5, i +1.0f,		j +0.0f,		k +1.0f,		.0f, stepY, .0f);
				cube.arrayEdgeLerpRatio[6] = mFunction_ComputeEdgeLerpRatio(6, i + 0.0f,		j +1.0f,		k+1.0f,			 stepX, .0f, .0f);
				cube.arrayEdgeLerpRatio[7] = mFunction_ComputeEdgeLerpRatio(7, i + 0.0f,		j + 0.0f,	k+1.0f,			.0f, stepY, .0f);
				cube.arrayEdgeLerpRatio[8] = mFunction_ComputeEdgeLerpRatio(8, i + 0.0f,		j + 0.0f,	k + 0.0f,		.0f, .0f, stepZ);
				cube.arrayEdgeLerpRatio[9] = mFunction_ComputeEdgeLerpRatio(9, i+1.0f,		j + 0.0f,	k + 0.0f,		.0f, .0f, stepZ);
				cube.arrayEdgeLerpRatio[10] = mFunction_ComputeEdgeLerpRatio(10, i+0.0f,	j+1.0f,		k + 0.0f,		.0f, .0f, stepZ);
				cube.arrayEdgeLerpRatio[11] = mFunction_ComputeEdgeLerpRatio(11, i+1.0f,	j+1.0f,		k + 0.0f,		.0f, .0f, stepZ);

				//Generate new triangle for this NON-TRIVIAL CUBE
				mFunction_MarchingCubeGenTriangles(cube);

			}
		}
	}

}

//intersecting iso-surface and cube edge.
//find the point where sample value suddenly changes(from 0 to 1, or 1 to 0)
inline float IMarchingCubeMeshReconstructor::mFunction_ComputeEdgeLerpRatio(int edgeID, float start_i, float start_j, float start_k, float stepX, float stepY, float stepZ)
{
		/*
		the cube define in Marching Cube[Lorensen 1987]

	     7 ________6
	      /|		   /|
	3  /_____ 2 /  |
	   |	   |4       |  / 5
	   |/_______ |/
	0				1

	Y|
	  |    /Z
	  |  /
	  |/___________X

		*/

		//calculate LERP ratio of point on edges
		/*	{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
		{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
		{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 } };*/

	//find the intersect point of cube-edge and iso-surface
	//sample multiple time to rougly estimate the intersect position
	//(where the value happen to exceed BinarizedThreshold)
	float edgeStartVal = mFunction_Sample(start_i, start_j, start_k);
	bool isInitValLargerThanThreshold = bool(edgeStartVal > c_SampleBinarizationThreshold);
	int maxStepCount = int(max(max(mResampleScaleX, mResampleScaleY), mResampleScaleZ));

	for (int stepCount=0; stepCount<maxStepCount;++stepCount)
	{
		float val = mFunction_Sample(start_i + stepX * stepCount, start_j + stepY * stepCount, start_k + stepZ *stepCount);
		bool isCurrentValLessThanThreshold = bool(val < c_SampleBinarizationThreshold);
		//sample value go to the other side of threshold
		if (isInitValLargerThanThreshold==isCurrentValLessThanThreshold)
		{
			switch (edgeID)
			{
			case 0:case 2:case 4: case 6: return stepX * stepCount;
			case 1:case 3:case 5: case 7: return stepY * stepCount;
			case 8:case 9:case 10:case 11:return stepZ * stepCount;
			default:return 1.0f;
			}
		}
	}

	return 1.0f;
}

void IMarchingCubeMeshReconstructor::mFunction_MarchingCubeGenTriangles(const N_NonTrivialCube& cube)
{
	//get triangle case (edge list)
	const N_MCTriangleCase& triCase = c_MarchingCubeTriangleCase[cube.triangleCaseIndex];


	//---------GENERATE TRIANGLES-------------
	/*	{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
	{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
	{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 } };*/

	//compute the minimum position of current cube
	float  cw = mResampledCubeWidth;
	float ch = mResampledCubeHeight;
	float cd = mResampledCubeDepth;
	float halfW = m_pVoxelizedModel->GetModelWidth() / 2.0f;//half width of the whole model
	float halfH = m_pVoxelizedModel->GetModelHeight() / 2.0f;//half height of the whole model
	float halfD = m_pVoxelizedModel->GetModelDepth() / 2.0f;//half depth of the whole model
	NVECTOR3 basePos =
	{
		cube.cubeIndexX * cw - halfW,
		cube.cubeIndexY * ch - halfH,
		cube.cubeIndexZ * cd - halfD
	};

	//1. coordinates of 8 cube vertices 
	NVECTOR3 v[8] =
	{
		basePos + NVECTOR3(0,0,0) ,
		basePos + NVECTOR3(cw,0,0),
		basePos + NVECTOR3(cw,ch,0),
		basePos + NVECTOR3(0,ch,0),
		basePos + NVECTOR3(0,0,cd),
		basePos + NVECTOR3(cw,0,cd),
		basePos + NVECTOR3(cw,ch,cd),
		basePos + NVECTOR3(0,ch,cd),
	};

	//2. then coordinates of points on edges
	NVECTOR3 pointOnEdge[12];
	for (int i = 0; i < 12; ++i)
	{
		NVECTOR3 start = v[c_edgeList[i][0]];
		NVECTOR3 end = v[c_edgeList[i][1]];
		pointOnEdge[i] = Lerp(start, end, cube.arrayEdgeLerpRatio[i]);
	}


	//3. use 'triangle case array' to generate triangles

	for (int i = 0; i < 16; i += 3)
	{
		if (triCase.index[i] == -1)break;
		//output to list 

		mVertexList.push_back(pointOnEdge[triCase.index[i]]);
		mVertexList.push_back(pointOnEdge[triCase.index[i + 2]]);
		mVertexList.push_back(pointOnEdge[triCase.index[i + 1]]);
	}
}

//new point will be on cube edges in the following array
const int IMarchingCubeMeshReconstructor::c_edgeList[12][2] = 
{ 
	{ 0,1 }, { 1,2 }, { 3,2 }, { 0,3 },
	{ 4,5 }, { 5,6 }, { 7,6 }, { 4,7 },
	{ 0,4 }, { 1,5 }, { 3,7 }, { 2,6 } 
};

//plz refer to [Lorensen 1987] for more detail. in the paper, edges are indexed from 1 to 12
//but in actual implementation, it should be 0 - 11.
const IMarchingCubeMeshReconstructor::N_MCTriangleCase IMarchingCubeMeshReconstructor::c_MarchingCubeTriangleCase[256] =
{
	{ { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 0 0 */
	{ { 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 1 1 */
	{ { 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 2 1 */
	{ { 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 3 2 */
	{ { 1, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 4 1 */
	{ { 0, 3, 8, 1, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 5 3 */
	{ { 9, 11, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 6 2 */
	{ { 2, 3, 8, 2, 8, 11, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1 } }, /* 7 5 */
	{ { 3, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 8 1 */
	{ { 0, 2, 10, 8, 0, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 9 2 */
	{ { 1, 0, 9, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 10 3 */
	{ { 1, 2, 10, 1, 10, 9, 9, 10, 8, -1, -1, -1, -1, -1, -1, -1 } }, /* 11 5 */
	{ { 3, 1, 11, 10, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 12 2 */
	{ { 0, 1, 11, 0, 11, 8, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1 } }, /* 13 5 */
	{ { 3, 0, 9, 3, 9, 10, 10, 9, 11, -1, -1, -1, -1, -1, -1, -1 } }, /* 14 5 */
	{ { 9, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 15 8 */
	{ { 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 16 1 */
	{ { 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 17 2 */
	{ { 0, 9, 1, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 18 3 */
	{ { 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1 } }, /* 19 5 */
	{ { 1, 11, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 20 4 */
	{ { 3, 7, 4, 3, 4, 0, 1, 11, 2, -1, -1, -1, -1, -1, -1, -1 } }, /* 21 7 */
	{ { 9, 11, 2, 9, 2, 0, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1 } }, /* 22 7 */
	{ { 2, 9, 11, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1 } }, /* 23 14 */
	{ { 8, 7, 4, 3, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 24 3 */
	{ { 10, 7, 4, 10, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1 } }, /* 25 5 */
	{ { 9, 1, 0, 8, 7, 4, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1 } }, /* 26 6 */
	{ { 4, 10, 7, 9, 10, 4, 9, 2, 10, 9, 1, 2, -1, -1, -1, -1 } }, /* 27 9 */
	{ { 3, 1, 11, 3, 11, 10, 7, 4, 8, -1, -1, -1, -1, -1, -1, -1 } }, /* 28 7 */
	{ { 1, 11, 10, 1, 10, 4, 1, 4, 0, 7, 4, 10, -1, -1, -1, -1 } }, /* 29 11 */
	{ { 4, 8, 7, 9, 10, 0, 9, 11, 10, 10, 3, 0, -1, -1, -1, -1 } }, /* 30 12 */
	{ { 4, 10, 7, 4, 9, 10, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1 } }, /* 31 5 */
	{ { 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 32 1 */
	{ { 9, 4, 5, 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 33 3 */
	{ { 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 34 2 */
	{ { 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1 } }, /* 35 5 */
	{ { 1, 11, 2, 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 36 3 */
	{ { 3, 8, 0, 1, 11, 2, 4, 5, 9, -1, -1, -1, -1, -1, -1, -1 } }, /* 37 6 */
	{ { 5, 11, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1 } }, /* 38 5 */
	{ { 2, 5, 11, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1 } }, /* 39 9 */
	{ { 9, 4, 5, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 40 4 */
	{ { 0, 2, 10, 0, 10, 8, 4, 5, 9, -1, -1, -1, -1, -1, -1, -1 } }, /* 41 7 */
	{ { 0, 4, 5, 0, 5, 1, 2, 10, 3, -1, -1, -1, -1, -1, -1, -1 } }, /* 42 7 */
	{ { 2, 5, 1, 2, 8, 5, 2, 10, 8, 4, 5, 8, -1, -1, -1, -1 } }, /* 43 11 */
	{ { 11, 10, 3, 11, 3, 1, 9, 4, 5, -1, -1, -1, -1, -1, -1, -1 } }, /* 44 7 */
	{ { 4, 5, 9, 0, 1, 8, 8, 1, 11, 8, 11, 10, -1, -1, -1, -1 } }, /* 45 12 */
	{ { 5, 0, 4, 5, 10, 0, 5, 11, 10, 10, 3, 0, -1, -1, -1, -1 } }, /* 46 14 */
	{ { 5, 8, 4, 5, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1 } }, /* 47 5 */
	{ { 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 48 2 */
	{ { 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1 } }, /* 49 5 */
	{ { 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1 } }, /* 50 5 */
	{ { 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 51 8 */
	{ { 9, 8, 7, 9, 7, 5, 11, 2, 1, -1, -1, -1, -1, -1, -1, -1 } }, /* 52 7 */
	{ { 11, 2, 1, 9, 0, 5, 5, 0, 3, 5, 3, 7, -1, -1, -1, -1 } }, /* 53 12 */
	{ { 8, 2, 0, 8, 5, 2, 8, 7, 5, 11, 2, 5, -1, -1, -1, -1 } }, /* 54 11 */
	{ { 2, 5, 11, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1 } }, /* 55 5 */
	{ { 7, 5, 9, 7, 9, 8, 3, 2, 10, -1, -1, -1, -1, -1, -1, -1 } }, /* 56 7 */
	{ { 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 10, 7, -1, -1, -1, -1 } }, /* 57 14 */
	{ { 2, 10, 3, 0, 8, 1, 1, 8, 7, 1, 7, 5, -1, -1, -1, -1 } }, /* 58 12 */
	{ { 10, 1, 2, 10, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1 } }, /* 59 5 */
	{ { 9, 8, 5, 8, 7, 5, 11, 3, 1, 11, 10, 3, -1, -1, -1, -1 } }, /* 60 10 */
	{ { 5, 0, 7, 5, 9, 0, 7, 0, 10, 1, 11, 0, 10, 0, 11, -1 } }, /* 61 7 */
	{ { 10, 0, 11, 10, 3, 0, 11, 0, 5, 8, 7, 0, 5, 0, 7, -1 } }, /* 62 7 */
	{ { 10, 5, 11, 7, 5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 63 2 */
	{ { 11, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 64 1 */
	{ { 0, 3, 8, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 65 4 */
	{ { 9, 1, 0, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 66 3 */
	{ { 1, 3, 8, 1, 8, 9, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1 } }, /* 67 7 */
	{ { 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 68 2 */
	{ { 1, 5, 6, 1, 6, 2, 3, 8, 0, -1, -1, -1, -1, -1, -1, -1 } }, /* 69 7 */
	{ { 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1 } }, /* 70 5 */
	{ { 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1 } }, /* 71 11 */
	{ { 2, 10, 3, 11, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 72 3 */
	{ { 10, 8, 0, 10, 0, 2, 11, 5, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 73 7 */
	{ { 0, 9, 1, 2, 10, 3, 5, 6, 11, -1, -1, -1, -1, -1, -1, -1 } }, /* 74 6 */
	{ { 5, 6, 11, 1, 2, 9, 9, 2, 10, 9, 10, 8, -1, -1, -1, -1 } }, /* 75 12 */
	{ { 6, 10, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1 } }, /* 76 5 */
	{ { 0, 10, 8, 0, 5, 10, 0, 1, 5, 5, 6, 10, -1, -1, -1, -1 } }, /* 77 14 */
	{ { 3, 6, 10, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1 } }, /* 78 9 */
	{ { 6, 9, 5, 6, 10, 9, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1 } }, /* 79 5 */
	{ { 5, 6, 11, 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 80 3 */
	{ { 4, 0, 3, 4, 3, 7, 6, 11, 5, -1, -1, -1, -1, -1, -1, -1 } }, /* 81 7 */
	{ { 1, 0, 9, 5, 6, 11, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1 } }, /* 82 6 */
	{ { 11, 5, 6, 1, 7, 9, 1, 3, 7, 7, 4, 9, -1, -1, -1, -1 } }, /* 83 12 */
	{ { 6, 2, 1, 6, 1, 5, 4, 8, 7, -1, -1, -1, -1, -1, -1, -1 } }, /* 84 7 */
	{ { 1, 5, 2, 5, 6, 2, 3, 4, 0, 3, 7, 4, -1, -1, -1, -1 } }, /* 85 10 */
	{ { 8, 7, 4, 9, 5, 0, 0, 5, 6, 0, 6, 2, -1, -1, -1, -1 } }, /* 86 12 */
	{ { 7, 9, 3, 7, 4, 9, 3, 9, 2, 5, 6, 9, 2, 9, 6, -1 } }, /* 87 7 */
	{ { 3, 2, 10, 7, 4, 8, 11, 5, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 88 6 */
	{ { 5, 6, 11, 4, 2, 7, 4, 0, 2, 2, 10, 7, -1, -1, -1, -1 } }, /* 89 12 */
	{ { 0, 9, 1, 4, 8, 7, 2, 10, 3, 5, 6, 11, -1, -1, -1, -1 } }, /* 90 13 */
	{ { 9, 1, 2, 9, 2, 10, 9, 10, 4, 7, 4, 10, 5, 6, 11, -1 } }, /* 91 6 */
	{ { 8, 7, 4, 3, 5, 10, 3, 1, 5, 5, 6, 10, -1, -1, -1, -1 } }, /* 92 12 */
	{ { 5, 10, 1, 5, 6, 10, 1, 10, 0, 7, 4, 10, 0, 10, 4, -1 } }, /* 93 7 */
	{ { 0, 9, 5, 0, 5, 6, 0, 6, 3, 10, 3, 6, 8, 7, 4, -1 } }, /* 94 6 */
	{ { 6, 9, 5, 6, 10, 9, 4, 9, 7, 7, 9, 10, -1, -1, -1, -1 } }, /* 95 3 */
	{ { 11, 9, 4, 6, 11, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 96 2 */
	{ { 4, 6, 11, 4, 11, 9, 0, 3, 8, -1, -1, -1, -1, -1, -1, -1 } }, /* 97 7 */
	{ { 11, 1, 0, 11, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1 } }, /* 98 5 */
	{ { 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 11, 1, -1, -1, -1, -1 } }, /* 99 14 */
	{ { 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 100 5 */
	{ { 3, 8, 0, 1, 9, 2, 2, 9, 4, 2, 4, 6, -1, -1, -1, -1 } }, /* 101 12 */
	{ { 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 102 8 */
	{ { 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1 } }, /* 103 5 */
	{ { 11, 9, 4, 11, 4, 6, 10, 3, 2, -1, -1, -1, -1, -1, -1, -1 } }, /* 104 7 */
	{ { 0, 2, 8, 2, 10, 8, 4, 11, 9, 4, 6, 11, -1, -1, -1, -1 } }, /* 105 10 */
	{ { 3, 2, 10, 0, 6, 1, 0, 4, 6, 6, 11, 1, -1, -1, -1, -1 } }, /* 106 12 */
	{ { 6, 1, 4, 6, 11, 1, 4, 1, 8, 2, 10, 1, 8, 1, 10, -1 } }, /* 107 7 */
	{ { 9, 4, 6, 9, 6, 3, 9, 3, 1, 10, 3, 6, -1, -1, -1, -1 } }, /* 108 11 */
	{ { 8, 1, 10, 8, 0, 1, 10, 1, 6, 9, 4, 1, 6, 1, 4, -1 } }, /* 109 7 */
	{ { 3, 6, 10, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 110 5 */
	{ { 6, 8, 4, 10, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 111 2 */
	{ { 7, 6, 11, 7, 11, 8, 8, 11, 9, -1, -1, -1, -1, -1, -1, -1 } }, /* 112 5 */
	{ { 0, 3, 7, 0, 7, 11, 0, 11, 9, 6, 11, 7, -1, -1, -1, -1 } }, /* 113 11 */
	{ { 11, 7, 6, 1, 7, 11, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1 } }, /* 114 9 */
	{ { 11, 7, 6, 11, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1 } }, /* 115 5 */
	{ { 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1 } }, /* 116 14 */
	{ { 2, 9, 6, 2, 1, 9, 6, 9, 7, 0, 3, 9, 7, 9, 3, -1 } }, /* 117 7 */
	{ { 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1 } }, /* 118 5 */
	{ { 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 119 2 */
	{ { 2, 10, 3, 11, 8, 6, 11, 9, 8, 8, 7, 6, -1, -1, -1, -1 } }, /* 120 12 */
	{ { 2, 7, 0, 2, 10, 7, 0, 7, 9, 6, 11, 7, 9, 7, 11, -1 } }, /* 121 7 */
	{ { 1, 0, 8, 1, 8, 7, 1, 7, 11, 6, 11, 7, 2, 10, 3, -1 } }, /* 122 6 */
	{ { 10, 1, 2, 10, 7, 1, 11, 1, 6, 6, 1, 7, -1, -1, -1, -1 } }, /* 123 3 */
	{ { 8, 6, 9, 8, 7, 6, 9, 6, 1, 10, 3, 6, 1, 6, 3, -1 } }, /* 124 7 */
	{ { 0, 1, 9, 10, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 125 4 */
	{ { 7, 0, 8, 7, 6, 0, 3, 0, 10, 10, 0, 6, -1, -1, -1, -1 } }, /* 126 3 */
	{ { 7, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 127 1 */
	{ { 7, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 128 1 */
	{ { 3, 8, 0, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 129 3 */
	{ { 0, 9, 1, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 130 4 */
	{ { 8, 9, 1, 8, 1, 3, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1 } }, /* 131 7 */
	{ { 11, 2, 1, 6, 7, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 132 3 */
	{ { 1, 11, 2, 3, 8, 0, 6, 7, 10, -1, -1, -1, -1, -1, -1, -1 } }, /* 133 6 */
	{ { 2, 0, 9, 2, 9, 11, 6, 7, 10, -1, -1, -1, -1, -1, -1, -1 } }, /* 134 7 */
	{ { 6, 7, 10, 2, 3, 11, 11, 3, 8, 11, 8, 9, -1, -1, -1, -1 } }, /* 135 12 */
	{ { 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 136 2 */
	{ { 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1 } }, /* 137 5 */
	{ { 2, 6, 7, 2, 7, 3, 0, 9, 1, -1, -1, -1, -1, -1, -1, -1 } }, /* 138 7 */
	{ { 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1 } }, /* 139 14 */
	{ { 11, 6, 7, 11, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1 } }, /* 140 5 */
	{ { 11, 6, 7, 1, 11, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1 } }, /* 141 9 */
	{ { 0, 7, 3, 0, 11, 7, 0, 9, 11, 6, 7, 11, -1, -1, -1, -1 } }, /* 142 11 */
	{ { 7, 11, 6, 7, 8, 11, 8, 9, 11, -1, -1, -1, -1, -1, -1, -1 } }, /* 143 5 */
	{ { 6, 4, 8, 10, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 144 2 */
	{ { 3, 10, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1 } }, /* 145 5 */
	{ { 8, 10, 6, 8, 6, 4, 9, 1, 0, -1, -1, -1, -1, -1, -1, -1 } }, /* 146 7 */
	{ { 9, 6, 4, 9, 3, 6, 9, 1, 3, 10, 6, 3, -1, -1, -1, -1 } }, /* 147 11 */
	{ { 6, 4, 8, 6, 8, 10, 2, 1, 11, -1, -1, -1, -1, -1, -1, -1 } }, /* 148 7 */
	{ { 1, 11, 2, 3, 10, 0, 0, 10, 6, 0, 6, 4, -1, -1, -1, -1 } }, /* 149 12 */
	{ { 4, 8, 10, 4, 10, 6, 0, 9, 2, 2, 9, 11, -1, -1, -1, -1 } }, /* 150 10 */
	{ { 11, 3, 9, 11, 2, 3, 9, 3, 4, 10, 6, 3, 4, 3, 6, -1 } }, /* 151 7 */
	{ { 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 152 5 */
	{ { 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 153 8 */
	{ { 1, 0, 9, 2, 4, 3, 2, 6, 4, 4, 8, 3, -1, -1, -1, -1 } }, /* 154 12 */
	{ { 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1 } }, /* 155 5 */
	{ { 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 11, -1, -1, -1, -1 } }, /* 156 14 */
	{ { 11, 0, 1, 11, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1 } }, /* 157 5 */
	{ { 4, 3, 6, 4, 8, 3, 6, 3, 11, 0, 9, 3, 11, 3, 9, -1 } }, /* 158 7 */
	{ { 11, 4, 9, 6, 4, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 159 2 */
	{ { 4, 5, 9, 7, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 160 3 */
	{ { 0, 3, 8, 4, 5, 9, 10, 6, 7, -1, -1, -1, -1, -1, -1, -1 } }, /* 161 6 */
	{ { 5, 1, 0, 5, 0, 4, 7, 10, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 162 7 */
	{ { 10, 6, 7, 8, 4, 3, 3, 4, 5, 3, 5, 1, -1, -1, -1, -1 } }, /* 163 12 */
	{ { 9, 4, 5, 11, 2, 1, 7, 10, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 164 6 */
	{ { 6, 7, 10, 1, 11, 2, 0, 3, 8, 4, 5, 9, -1, -1, -1, -1 } }, /* 165 13 */
	{ { 7, 10, 6, 5, 11, 4, 4, 11, 2, 4, 2, 0, -1, -1, -1, -1 } }, /* 166 12 */
	{ { 3, 8, 4, 3, 4, 5, 3, 5, 2, 11, 2, 5, 10, 6, 7, -1 } }, /* 167 6 */
	{ { 7, 3, 2, 7, 2, 6, 5, 9, 4, -1, -1, -1, -1, -1, -1, -1 } }, /* 168 7 */
	{ { 9, 4, 5, 0, 6, 8, 0, 2, 6, 6, 7, 8, -1, -1, -1, -1 } }, /* 169 12 */
	{ { 3, 2, 6, 3, 6, 7, 1, 0, 5, 5, 0, 4, -1, -1, -1, -1 } }, /* 170 10 */
	{ { 6, 8, 2, 6, 7, 8, 2, 8, 1, 4, 5, 8, 1, 8, 5, -1 } }, /* 171 7 */
	{ { 9, 4, 5, 11, 6, 1, 1, 6, 7, 1, 7, 3, -1, -1, -1, -1 } }, /* 172 12 */
	{ { 1, 11, 6, 1, 6, 7, 1, 7, 0, 8, 0, 7, 9, 4, 5, -1 } }, /* 173 6 */
	{ { 4, 11, 0, 4, 5, 11, 0, 11, 3, 6, 7, 11, 3, 11, 7, -1 } }, /* 174 7 */
	{ { 7, 11, 6, 7, 8, 11, 5, 11, 4, 4, 11, 8, -1, -1, -1, -1 } }, /* 175 3 */
	{ { 6, 5, 9, 6, 9, 10, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1 } }, /* 176 5 */
	{ { 3, 10, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1 } }, /* 177 9 */
	{ { 0, 8, 10, 0, 10, 5, 0, 5, 1, 5, 10, 6, -1, -1, -1, -1 } }, /* 178 14 */
	{ { 6, 3, 10, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1 } }, /* 179 5 */
	{ { 1, 11, 2, 9, 10, 5, 9, 8, 10, 10, 6, 5, -1, -1, -1, -1 } }, /* 180 12 */
	{ { 0, 3, 10, 0, 10, 6, 0, 6, 9, 5, 9, 6, 1, 11, 2, -1 } }, /* 181 6 */
	{ { 10, 5, 8, 10, 6, 5, 8, 5, 0, 11, 2, 5, 0, 5, 2, -1 } }, /* 182 7 */
	{ { 6, 3, 10, 6, 5, 3, 2, 3, 11, 11, 3, 5, -1, -1, -1, -1 } }, /* 183 3 */
	{ { 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1 } }, /* 184 11 */
	{ { 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1 } }, /* 185 5 */
	{ { 1, 8, 5, 1, 0, 8, 5, 8, 6, 3, 2, 8, 6, 8, 2, -1 } }, /* 186 7 */
	{ { 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 187 2 */
	{ { 1, 6, 3, 1, 11, 6, 3, 6, 8, 5, 9, 6, 8, 6, 9, -1 } }, /* 188 7 */
	{ { 11, 0, 1, 11, 6, 0, 9, 0, 5, 5, 0, 6, -1, -1, -1, -1 } }, /* 189 3 */
	{ { 0, 8, 3, 5, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 190 4 */
	{ { 11, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 191 1 */
	{ { 10, 11, 5, 7, 10, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 192 2 */
	{ { 10, 11, 5, 10, 5, 7, 8, 0, 3, -1, -1, -1, -1, -1, -1, -1 } }, /* 193 7 */
	{ { 5, 7, 10, 5, 10, 11, 1, 0, 9, -1, -1, -1, -1, -1, -1, -1 } }, /* 194 7 */
	{ { 11, 5, 7, 11, 7, 10, 9, 1, 8, 8, 1, 3, -1, -1, -1, -1 } }, /* 195 10 */
	{ { 10, 2, 1, 10, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1 } }, /* 196 5 */
	{ { 0, 3, 8, 1, 7, 2, 1, 5, 7, 7, 10, 2, -1, -1, -1, -1 } }, /* 197 12 */
	{ { 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 10, -1, -1, -1, -1 } }, /* 198 14 */
	{ { 7, 2, 5, 7, 10, 2, 5, 2, 9, 3, 8, 2, 9, 2, 8, -1 } }, /* 199 7 */
	{ { 2, 11, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1 } }, /* 200 5 */
	{ { 8, 0, 2, 8, 2, 5, 8, 5, 7, 11, 5, 2, -1, -1, -1, -1 } }, /* 201 11 */
	{ { 9, 1, 0, 5, 3, 11, 5, 7, 3, 3, 2, 11, -1, -1, -1, -1 } }, /* 202 12 */
	{ { 9, 2, 8, 9, 1, 2, 8, 2, 7, 11, 5, 2, 7, 2, 5, -1 } }, /* 203 7 */
	{ { 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 204 8 */
	{ { 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1 } }, /* 205 5 */
	{ { 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1 } }, /* 206 5 */
	{ { 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 207 2 */
	{ { 5, 4, 8, 5, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1, -1 } }, /* 208 5 */
	{ { 5, 4, 0, 5, 0, 10, 5, 10, 11, 10, 0, 3, -1, -1, -1, -1 } }, /* 209 14 */
	{ { 0, 9, 1, 8, 11, 4, 8, 10, 11, 11, 5, 4, -1, -1, -1, -1 } }, /* 210 12 */
	{ { 11, 4, 10, 11, 5, 4, 10, 4, 3, 9, 1, 4, 3, 4, 1, -1 } }, /* 211 7 */
	{ { 2, 1, 5, 2, 5, 8, 2, 8, 10, 4, 8, 5, -1, -1, -1, -1 } }, /* 212 11 */
	{ { 0, 10, 4, 0, 3, 10, 4, 10, 5, 2, 1, 10, 5, 10, 1, -1 } }, /* 213 7 */
	{ { 0, 5, 2, 0, 9, 5, 2, 5, 10, 4, 8, 5, 10, 5, 8, -1 } }, /* 214 7 */
	{ { 9, 5, 4, 2, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 215 4 */
	{ { 2, 11, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1 } }, /* 216 9 */
	{ { 5, 2, 11, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1 } }, /* 217 5 */
	{ { 3, 2, 11, 3, 11, 5, 3, 5, 8, 4, 8, 5, 0, 9, 1, -1 } }, /* 218 6 */
	{ { 5, 2, 11, 5, 4, 2, 1, 2, 9, 9, 2, 4, -1, -1, -1, -1 } }, /* 219 3 */
	{ { 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1 } }, /* 220 5 */
	{ { 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 221 2 */
	{ { 8, 5, 4, 8, 3, 5, 9, 5, 0, 0, 5, 3, -1, -1, -1, -1 } }, /* 222 3 */
	{ { 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 223 1 */
	{ { 4, 7, 10, 4, 10, 9, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1 } }, /* 224 5 */
	{ { 0, 3, 8, 4, 7, 9, 9, 7, 10, 9, 10, 11, -1, -1, -1, -1 } }, /* 225 12 */
	{ { 1, 10, 11, 1, 4, 10, 1, 0, 4, 7, 10, 4, -1, -1, -1, -1 } }, /* 226 11 */
	{ { 3, 4, 1, 3, 8, 4, 1, 4, 11, 7, 10, 4, 11, 4, 10, -1 } }, /* 227 7 */
	{ { 4, 7, 10, 9, 4, 10, 9, 10, 2, 9, 2, 1, -1, -1, -1, -1 } }, /* 228 9 */
	{ { 9, 4, 7, 9, 7, 10, 9, 10, 1, 2, 1, 10, 0, 3, 8, -1 } }, /* 229 6 */
	{ { 10, 4, 7, 10, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1 } }, /* 230 5 */
	{ { 10, 4, 7, 10, 2, 4, 8, 4, 3, 3, 4, 2, -1, -1, -1, -1 } }, /* 231 3 */
	{ { 2, 11, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1 } }, /* 232 14 */
	{ { 9, 7, 11, 9, 4, 7, 11, 7, 2, 8, 0, 7, 2, 7, 0, -1 } }, /* 233 7 */
	{ { 3, 11, 7, 3, 2, 11, 7, 11, 4, 1, 0, 11, 4, 11, 0, -1 } }, /* 234 7 */
	{ { 1, 2, 11, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 235 4 */
	{ { 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1 } }, /* 236 5 */
	{ { 4, 1, 9, 4, 7, 1, 0, 1, 8, 8, 1, 7, -1, -1, -1, -1 } }, /* 237 3 */
	{ { 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 238 2 */
	{ { 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 239 1 */
	{ { 9, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 240 8 */
	{ { 3, 9, 0, 3, 10, 9, 10, 11, 9, -1, -1, -1, -1, -1, -1, -1 } }, /* 241 5 */
	{ { 0, 11, 1, 0, 8, 11, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1 } }, /* 242 5 */
	{ { 3, 11, 1, 10, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 243 2 */
	{ { 1, 10, 2, 1, 9, 10, 9, 8, 10, -1, -1, -1, -1, -1, -1, -1 } }, /* 244 5 */
	{ { 3, 9, 0, 3, 10, 9, 1, 9, 2, 2, 9, 10, -1, -1, -1, -1 } }, /* 245 3 */
	{ { 0, 10, 2, 8, 10, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 246 2 */
	{ { 3, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 247 1 */
	{ { 2, 8, 3, 2, 11, 8, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1 } }, /* 248 5 */
	{ { 9, 2, 11, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 249 2 */
	{ { 2, 8, 3, 2, 11, 8, 0, 8, 1, 1, 8, 11, -1, -1, -1, -1 } }, /* 250 3 */
	{ { 1, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 251 1 */
	{ { 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 252 2 */
	{ { 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 253 1 */
	{ { 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, /* 254 1 */
	{ { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } } /* 255 0 */
};