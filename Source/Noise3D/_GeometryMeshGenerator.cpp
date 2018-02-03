
/***********************************************************************

					Class : Geometry Mesh Generator

			Desc: Procedural geometry generator

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

void IGeometryMeshGenerator::CreatePlane(float fWidth, float fDepth, UINT iRowCount, UINT iColumnCount, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, 0, fDepth / 2),
		NVECTOR3(fWidth / (float)(iColumnCount - 1), 0, 0),//tangent
		NVECTOR3(0, 0, -fDepth / (float)(iRowCount - 1)),
		iRowCount,
		iColumnCount, 
		0,
		outVerticeList,
		outIndicesList);
}

void IGeometryMeshGenerator::CreateBox(float fWidth, float fHeight, float fDepth, UINT iDepthStep, UINT iWidthStep, UINT iHeightStep, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{
	/*
	Y  |
	|      /  Z
	|	 /
	|  /
	|/___________ X

	*/

	//Build 6 Quad
	int tmpBaseIndex;
	//BOTTOM- NORMAL√
	float tmpStep1 = fWidth / (float)(iWidthStep - 1);
	float tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = 0;
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(tmpStep1, 0, 0),
		NVECTOR3(0, 0, tmpStep2),
		iWidthStep,
		iDepthStep,
		tmpBaseIndex, 
		outVerticeList,
		outIndicesList);

	//TOP- NORMAL√
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpStep1 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, fHeight / 2, fDepth / 2),
		NVECTOR3(tmpStep1, 0, 0),
		NVECTOR3(0, 0, -tmpStep2),
		iDepthStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//LEFT- NORMAL√
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(0, 0, -tmpStep2),
		iDepthStep,
		iHeightStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//RIGHT- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(0, 0, tmpStep2),
		iHeightStep,
		iDepthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);


	//FRONT- NORMAL√
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//BACK- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth / 2, -fHeight / 2, fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(-tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);
}

void IGeometryMeshGenerator::CreateSphere(float fRadius, UINT iColumnCount, UINT iRingCount, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{

	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be 2 cluster of vertices(at same position, different texcoord)
	//the first column will be duplicated to achieve adequate texture mapping

	//top/bottom vertex are clustered into one position, but with different texture coordinate
	UINT tmpVertexCount = (iColumnCount + 1) * (iRingCount + 2);
	NVECTOR3* tmpV = new NVECTOR3[tmpVertexCount];
	NVECTOR2* tmpTexCoord = new NVECTOR2[tmpVertexCount];
	NVECTOR3* tmpTangent = new NVECTOR3[tmpVertexCount];


#pragma region GenerateVertex

	float	tmpX,tmpY,tmpZ,tmpRingRadius;

	//Calculate the Step length (步长)
	float	StepLength_AngleY =		MATH_PI / (iRingCount +1); // distances between each level (ring)
	float StepLength_AngleXZ =		2*MATH_PI / iColumnCount;

	UINT k = 0;//for iteration
	//start to iterate
	for(int i =-1;i < int(iRingCount)+1 ;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for(int j = 0; j <int(iColumnCount)+1 ; j++)
		{
			/*//the Y coord of  current ring 
			tmpY = fRadius *sin( MATH_PI/2 - (i+1) *StepLength_AngleY);

			////Pythagoras theorem(勾股定理)
			tmpRingRadius = sqrtf(fRadius*fRadius - tmpY * tmpY); 

			////trigonometric function(三角函数)
			tmpX = tmpRingRadius * cos( j*StepLength_AngleXZ);

			//...
			tmpZ = tmpRingRadius * sin( j*StepLength_AngleXZ);

			//...
			tmpV[k] = NVECTOR3(tmpX,tmpY,tmpZ);

			//map the i,j to closed interval [0,1] respectively , to proceed a spheric texture wrapping
			tmpTexCoord[k] = NVECTOR2( (float)j/(iColumnCount),(float)i /(iRingCount-1));

			k++;*/
			//TOP || BOTTOM
			if (i == -1)
			{
				tmpX = 0;
				tmpZ = 0;
				tmpY = fRadius;
			}
			else if (i == iColumnCount)
			{
				tmpX = 0;
				tmpZ = 0;
				tmpY = -fRadius;
			}
			else
			{
				//the Y coord of  current ring 
				tmpY = fRadius *sin(MATH_PI / 2 - (i + 1) *StepLength_AngleY);
				// radius of current horizontal ring 
				tmpRingRadius = sqrtf(fRadius*fRadius - tmpY * tmpY);
				//compute x,z
				tmpX = tmpRingRadius * cos(j*StepLength_AngleXZ);
				tmpZ = tmpRingRadius * sin(j*StepLength_AngleXZ);
			}

			//store position in array
			tmpV[k] = NVECTOR3(tmpX, tmpY, tmpZ);
			//map the i,j to closed interval [0,1] respectively , to proceed a spheric texture wrapping
			tmpTexCoord[k] = NVECTOR2((float)j / (iColumnCount), (float)(i+1) / (iRingCount+1));
			//tangent need to be dealt with specially
			tmpTangent[k] = NVECTOR3(-sinf(j*StepLength_AngleXZ), 0, cos(j*StepLength_AngleXZ));
			k++;
		}
	}

	//add to Memory
	N_DefaultVertex tmpCompleteV;
	for(UINT i =0;i<tmpVertexCount;i++)
	{
		tmpCompleteV.Pos				= tmpV[i];
		tmpCompleteV.Normal		= NVECTOR3(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius);
		tmpCompleteV.Color			= 	NVECTOR4(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius,1.0f);
		tmpCompleteV.TexCoord	= tmpTexCoord[i];
		tmpCompleteV.Tangent		= tmpTangent[i];
		outVerticeList.push_back(tmpCompleteV);
	}

#pragma endregion GenerateVertex

#pragma region GenerateIndices
	//Generate Indices of a ball
	//every Ring grows a triangle net with lower level ring
	for (uint32_t i = 0; i<iRingCount + 1; ++i)
	{
		for (uint32_t j = 0; j<iColumnCount; ++j)
		{
			/*	
					v1	_____ v2
						|    /
						|  /
						|/		v3
		
			*/
			//one ring owns (iColumnCount +1) vertices, 'Cos the first column is copied
			outIndicesList.push_back(	i*			(iColumnCount+1)		+j		+0);
			outIndicesList.push_back(	i*			(iColumnCount+1)		+j		+1);
			outIndicesList.push_back(	(i+1)*	(iColumnCount	+1)		+j		+0);

			/*
						v4
					    /|
					  /  |
			v6		/___|	v5

			*/
			outIndicesList.push_back(	i*			(iColumnCount+1)		+j		+1);
			outIndicesList.push_back(	(i+1)*	(iColumnCount+1)		+j		+1);
			outIndicesList.push_back(	(i+1)*	(iColumnCount+1)		+j		+0);
		}
	}

	
#pragma endregion GenerateIndices

	delete tmpTexCoord;
	delete tmpV;
}

void IGeometryMeshGenerator::CreateCylinder(float fRadius, float fHeight, UINT iColumnCount, UINT iRingCount, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{

	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the last "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be restored in the last 2 position in this array
	//the first column will be duplicated to achieve adequate texture mapping
	NVECTOR3* tmpV;
	NVECTOR2* tmpTexCoord;
	UINT tmpVertexCount = (iColumnCount + 1) * (iRingCount + 2) + 2;
	tmpV = new NVECTOR3[tmpVertexCount];
	tmpTexCoord = new NVECTOR2[tmpVertexCount];
	tmpV[tmpVertexCount - 2] = NVECTOR3(NVECTOR3(0, fHeight / 2, 0));		//TOP vertex
	tmpV[tmpVertexCount - 1] = NVECTOR3(NVECTOR3(0, -fHeight / 2, 0));		//BOTTOM vertex
	tmpTexCoord[tmpVertexCount - 2] = NVECTOR2(0.5f, 0);			//TOP vertex
	tmpTexCoord[tmpVertexCount - 1] = NVECTOR2(0.5f, 1.0f);			//BOTTOM vertex
	UINT tmpVertexArrayIter = 0;

	float	tmpX, tmpY, tmpZ;

	//Calculate the Step length (步长)
	//the RINGS include "the top ring" and "the bottom ring"
	float	StepLength_Y = fHeight / (iRingCount - 1); // distances between each level (ring)
	float StepLength_Angle = 2 * MATH_PI / iColumnCount;


#pragma region GenerateVertex

	//start to iterate
	for (UINT i = 0;i < iRingCount;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for (UINT j = 0; j < iColumnCount + 1; j++)
		{

			tmpY = (fHeight / 2) - i *StepLength_Y;
			tmpX = fRadius * cos(j*StepLength_Angle);
			tmpZ = fRadius * sin(j*StepLength_Angle);
			tmpV[tmpVertexArrayIter] = NVECTOR3(tmpX, tmpY, tmpZ);

			//TexCoord generation, look for more detail in tech doc
			tmpTexCoord[tmpVertexArrayIter] = NVECTOR2((float)j / (iColumnCount - 1), tmpY / (fRadius * 2 + fHeight));

			++tmpVertexArrayIter;
		}
	}

	//要增加TOP/BOTTOM两个RING，因为NORMAL不一样，这个要指着上面
	for (UINT j = 0; j < iColumnCount + 1; j++)
	{
		tmpY = (fHeight / 2);
		tmpX = fRadius * cos(j*StepLength_Angle);
		tmpZ = fRadius * sin(j*StepLength_Angle);
		tmpV[tmpVertexArrayIter] = NVECTOR3(tmpX, tmpY, tmpZ);
		tmpTexCoord[tmpVertexArrayIter] = NVECTOR2((float)j / (iColumnCount - 1), tmpY / (fRadius * 2 + fHeight));
		++tmpVertexArrayIter;
	}

	for (UINT j = 0; j < iColumnCount + 1; j++)
	{
		tmpY = (-fHeight / 2);
		tmpX = fRadius * cos(j*StepLength_Angle);
		tmpZ = fRadius * sin(j*StepLength_Angle);
		tmpV[tmpVertexArrayIter] = NVECTOR3(tmpX, tmpY, tmpZ);
		tmpTexCoord[tmpVertexArrayIter] = NVECTOR2((float)j / (iColumnCount - 1), tmpY / (fRadius * 2 + fHeight));
		++tmpVertexArrayIter;
	}




	//------------Complete Vertex Format-----------
	//侧面(side Face) along with their normals
	for (UINT i = 0;i<(iColumnCount + 1)*iRingCount;i++)
	{
		N_DefaultVertex tmpCompleteV;
		tmpCompleteV.Pos = tmpV[i];
		tmpCompleteV.Normal = NVECTOR3(tmpV[i].x / fRadius, 0, tmpV[i].z / fRadius);
		tmpCompleteV.Tangent = NVECTOR3(-tmpCompleteV.Normal.z, 0, tmpCompleteV.Normal.x!=0.0? tmpCompleteV.Normal.x:0.001f);//mighty tangent algorithm= =
		tmpCompleteV.Color = NVECTOR4(tmpV[i].x / fRadius, tmpV[i].y / fRadius, tmpV[i].z / fRadius, 1.0f);
		tmpCompleteV.TexCoord = tmpTexCoord[i];
		outVerticeList.push_back(tmpCompleteV);
	}

	//TOP/BOTTOM ring along with their normals
	for (UINT i = (iColumnCount + 1)*iRingCount;i<(iColumnCount + 1)*(iRingCount + 2);i++)
	{
		N_DefaultVertex tmpCompleteV;
		tmpCompleteV.Pos = tmpV[i];

		//set the normal according the sign of Y coord
		tmpCompleteV.Normal = NVECTOR3(0.0f, (tmpV[i].y>0 ? 1.0f : -1.0f), 0);

		//!!!!!!!!!!!!!!!!! TANGENT CAN'T BE PARALLEL WITH Z AXIS (TOGETHER
		// WITH THE TOP VERTEX !!!!?????!!!!!   OR IT'LL BE BLACK  !!!!!!!!!!!!
		//tmpCompleteV.Tangent = NVECTOR3(0,0, (tmpV[i].y>0 ? -1.0f : 1.0f));
		//tmpCompleteV.Tangent = NVECTOR3(tmpCompleteV.Pos.x/fRadius,0 , tmpCompleteV.Pos.z / fRadius);
		tmpCompleteV.Tangent = NVECTOR3(1.0f, 0, 0);
		tmpCompleteV.Color = NVECTOR4(tmpV[i].x / fRadius, tmpV[i].y / fRadius, tmpV[i].z / fRadius, 1.0f);
		tmpCompleteV.TexCoord = tmpTexCoord[i];
		outVerticeList.push_back(tmpCompleteV);
	}

	
	N_DefaultVertex tmpCompleteV;
	//TOP/BOTTOM Vertex
	tmpCompleteV.Pos = tmpV[tmpVertexCount - 2];
	tmpCompleteV.Normal = NVECTOR3(0, 1.0f, 0);
	tmpCompleteV.Tangent = NVECTOR3(0.001f, 0, 0.0f);
	tmpCompleteV.Color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	tmpCompleteV.TexCoord = tmpTexCoord[tmpVertexCount - 2];
	outVerticeList.push_back(tmpCompleteV);

	tmpCompleteV.Pos = tmpV[tmpVertexCount - 1];
	tmpCompleteV.Normal = NVECTOR3(0, -1.0f, 0);
	tmpCompleteV.Tangent = NVECTOR3(0, 0, -1.0f);
	tmpCompleteV.Color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	tmpCompleteV.TexCoord = tmpTexCoord[tmpVertexCount - 1];
	outVerticeList.push_back(tmpCompleteV);

#pragma endregion GenerateVertex


#pragma region GenerateIndex

	//Generate Indices of a ball
	//every Ring grows a triangle net with lower level ring
	for (UINT i = 0; i<iRingCount - 1; i++)
	{
		for (UINT j = 0; j<iColumnCount; j++)
		{

			/*
			k	_____ k+1
			|    /
			|  /
			|/		k+2

			*/
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 0);
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 1);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 0);

			/*
			k+3
			/|
			/  |
			k+5	/___|	k+4

			*/
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 1);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 1);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 0);
		}
	}


	//deal with the TOP/BOTTOM
	for (UINT j = 0;j<iColumnCount;j++)
	{
		outIndicesList.push_back((iColumnCount + 1)*iRingCount + j);
		outIndicesList.push_back((iColumnCount + 1)*iRingCount + j + 1);
		outIndicesList.push_back(tmpVertexCount - 2);	//index of top vertex

		outIndicesList.push_back((iColumnCount + 1) * (iRingCount + 1) + j);
		outIndicesList.push_back((iColumnCount + 1) * (iRingCount + 1) + j + 1);
		outIndicesList.push_back(tmpVertexCount - 1); //index of bottom vertex
	}

#pragma endregion GenerateIndex

	delete tmpV;
	delete tmpTexCoord;
}

void IGeometryMeshGenerator::CreateSkyDome(float fRadiusXZ, float fHeight, UINT iColumnCount, UINT iRingCount, std::vector<N_SimpleVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{

	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be 2 cluster of vertices(at same position, different texcoord)
	//the first column will be duplicated to achieve adequate texture mapping

	//top/bottom vertex are clustered into one position, but with different texture coordinate
	UINT tmpVertexCount = (iColumnCount + 1) * (iRingCount + 2);
	NVECTOR3* tmpV = new NVECTOR3[tmpVertexCount];
	NVECTOR2* tmpTexCoord = new NVECTOR2[tmpVertexCount];

	//Calculate the Step length (步长)
	float	StepLength_AngleY = MATH_PI / (iRingCount + 1); // distances between each level (ring)
	float StepLength_AngleXZ = 2 * MATH_PI / iColumnCount;

#pragma region GenerateVertex

	//start to iterate
	UINT k = 0;
	float	tmpX, tmpY, tmpZ, tmpRingRadius;
	//each ring is parallel to the water plane (XZ)
	for (int i = -1;i < int(iRingCount)+1;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for (int j = 0; j < int(iColumnCount) + 1; j++)
		{
			//TOP || BOTTOM
			if (i == -1)
			{
				tmpX = 0;
				tmpZ = 0;
				tmpY = fHeight;
			}
			else if (i == iColumnCount)
			{
				tmpX = 0;
				tmpZ = 0;
				tmpY = -fHeight;
			}
			else
			{			
				//the Y coord of  current ring 
				tmpY = fHeight *sin(MATH_PI / 2 - (i + 1) *StepLength_AngleY);
				// radius of current horizontal ring 
				tmpRingRadius = fRadiusXZ* sqrtf(1 - (tmpY * tmpY) / (fHeight*fHeight));
				//compute x,z
				tmpX = tmpRingRadius * cos(j*StepLength_AngleXZ);
				tmpZ = tmpRingRadius * sin(j*StepLength_AngleXZ);
			}

			//store position in array
			tmpV[k] = NVECTOR3(tmpX, tmpY, tmpZ);
			//map the i,j to closed interval [0,1] respectively , to proceed a spheric texture wrapping
			tmpTexCoord[k] = NVECTOR2((float)j / (iColumnCount), (float)(i+1) / (iRingCount+1));
			k++;
		}
	}


	//add to output list
	for (UINT i = 0;i<tmpVertexCount;i++)
	{
		N_SimpleVertex tmpCompleteV;
		tmpCompleteV.Pos = tmpV[i];
		tmpCompleteV.Color = NVECTOR4(tmpV[i].x / fRadiusXZ, tmpV[i].y / fHeight, tmpV[i].z / fRadiusXZ, 1.0f);
		tmpCompleteV.TexCoord = tmpTexCoord[i];
		outVerticeList.push_back(tmpCompleteV);
	}

#pragma endregion GenerateVertex

#pragma region GenerateIndex

	//Generate Indices of a ball
	//deal with the middle
	//every Ring grows a triangle net with lower level ring
	//for (int i = 0; i<iRingCount - 1; i++)
	for(uint32_t i=0;i<iRingCount+1;++i)
	{
		for (uint32_t j = 0; j<iColumnCount; ++j)
		{
			/*
			k	_____ k+1
			    |    /
		    	|  /
				|/		k+2

			*/
			//rotating order is different from the one in mesh ,because sky dome face inside
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 0);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 0);
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 1);


			/*
			k+3
						/|
					  /  |
			k+5	/___|	k+4

			*/
			outIndicesList.push_back(i*			(iColumnCount + 1) + j + 1);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 0);
			outIndicesList.push_back((i + 1)*	(iColumnCount + 1) + j + 1);

		}
	}

#pragma endregion GenerateIndex

	delete tmpV;
	delete tmpTexCoord;

}

void IGeometryMeshGenerator::CreateSkyBox(float fWidth, float fHeight, float fDepth, std::vector<N_SimpleVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{
	//Build 6 Quad
	int tmpBaseIndex;
	UINT iWidthStep = 3, iDepthStep = 3, iHeightStep = 3;

	//BOTTOM- NORMAL√
	float tmpStep1 = fWidth / (float)(iWidthStep - 1);
	float tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = 0;
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(tmpStep1, 0, 0),
		NVECTOR3(0, 0, tmpStep2),
		iWidthStep,
		iDepthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//TOP- NORMAL√
	tmpStep1 = fDepth / (float)(iDepthStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, fHeight / 2, -fDepth / 2),
		NVECTOR3(0, 0, tmpStep1),
		NVECTOR3(tmpStep2, 0, 0),
		iDepthStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//LEFT- NORMAL√
	tmpStep1 = fDepth / (float)(iDepthStep - 1);
	tmpStep2 = fHeight / (float)(iHeightStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, 0, tmpStep1),
		NVECTOR3(0, tmpStep2, 0),
		iDepthStep,
		iHeightStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//RIGHT- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(0, 0, tmpStep2),
		iHeightStep,
		iDepthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);


	//FRONT- NORMAL√
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

	//BACK- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = -fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = outVerticeList.size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth / 2, -fHeight / 2, fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex,
		outVerticeList,
		outIndicesList);

}

/***********************************************************************
								P R I V A T E					                    
***********************************************************************/

inline void IGeometryMeshGenerator::mFunction_Build_A_Quad
(NVECTOR3 vOriginPoint, NVECTOR3 vBasisVector1, NVECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
{
	NVECTOR3 tmpNormal;
	N_DefaultVertex tmpCompleteV;
	D3DXVec3Cross(&tmpNormal, &vBasisVector1, &vBasisVector2);
	D3DXVec3Normalize(&tmpNormal, &tmpNormal);

	for (UINT i = 0;i < StepCount1;i++)
	{
		for (UINT j = 0;j < StepCount2;j++)
		{
			tmpCompleteV.Normal = tmpNormal;
			tmpCompleteV.Pos = NVECTOR3(vOriginPoint + (float)i*vBasisVector1 + (float)j*vBasisVector2);
			tmpCompleteV.Color = NVECTOR4(((float)i / StepCount1), ((float)j / StepCount2), 0.5f, 1.0f);
			NVECTOR3 normalizedTangent;
			D3DXVec3Normalize(&normalizedTangent,&vBasisVector1);
			tmpCompleteV.Tangent = normalizedTangent;
			tmpCompleteV.TexCoord = NVECTOR2((float)i / (StepCount1 - 1), ((float)j / StepCount2));
			outVerticeList.push_back(tmpCompleteV);
		}
	}

	for (UINT i = 0;i<StepCount1 - 1;i++)
	{
		for (UINT j = 0;j<StepCount2 - 1;j++)
		{
			//why use iBaseIndex : when we build things like a box , we need build 6 quads ,
			//thus inde offset is needed
			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j);
			outIndicesList.push_back(iBaseIndex + (i + 1)* StepCount2 + j);
			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j + 1);

			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j + 1);
			outIndicesList.push_back(iBaseIndex + (i + 1) *StepCount2 + j);
			outIndicesList.push_back(iBaseIndex + (i + 1)* StepCount2 + j + 1);
		}
	}

}

inline void	 IGeometryMeshGenerator::mFunction_Build_A_Quad
(NVECTOR3 vOriginPoint, NVECTOR3 vBasisVector1, NVECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex, std::vector<N_SimpleVertex>& outVerticeList, std::vector<UINT>& outIndicesList)
	// it is used to build a Quad , or say Rectangle . StepCount is similar to the count of sections
{
#pragma region GenerateVertex

	UINT i = 0, j = 0;
	NVECTOR3 tmpNormal;
	N_SimpleVertex tmpCompleteV;
	D3DXVec3Cross(&tmpNormal, &vBasisVector1, &vBasisVector2);
	D3DXVec3Normalize(&tmpNormal, &tmpNormal);

	for (i = 0;i < StepCount1;i++)
	{
		for (j = 0;j < StepCount2;j++)
		{
			tmpCompleteV.Pos = NVECTOR3(vOriginPoint + (float)i*vBasisVector1 + (float)j*vBasisVector2);
			tmpCompleteV.Color = NVECTOR4(((float)i / StepCount1), ((float)j / StepCount2), 0.5f, 1.0f);
			tmpCompleteV.TexCoord = NVECTOR2((float)i / (StepCount1 - 1), ((float)j / StepCount2));
			outVerticeList.push_back(tmpCompleteV);
		}
	}
#pragma endregion GenerateVertex


#pragma region GenerateIndex
	i = 0;j = 0;
	for (i = 0;i<StepCount1 - 1;i++)
	{
		for (j = 0;j<StepCount2 - 1;j++)
		{
			//why use iBaseIndex : when we build things like a box , we need build 6 quads ,
			//thus inde offset is needed
			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j);
			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j + 1);
			outIndicesList.push_back(iBaseIndex + (i + 1)* StepCount2 + j);
			//m_pIB_Mem_Sky->push_back(iBaseIndex + i *		StepCount2 + j + 1);

			outIndicesList.push_back(iBaseIndex + i *		StepCount2 + j + 1);
			outIndicesList.push_back(iBaseIndex + (i + 1)* StepCount2 + j + 1);
			outIndicesList.push_back(iBaseIndex + (i + 1) *StepCount2 + j);
			//m_pIB_Mem_Sky->push_back(iBaseIndex + (i + 1)* StepCount2 + j + 1);
		}
	}

#pragma endregion GenerateIndex

};
