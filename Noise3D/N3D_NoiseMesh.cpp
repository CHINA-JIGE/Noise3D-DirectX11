
/***********************************************************************

							类：NoiseMesh

				简述：封装了一个网格类，由SceneManager来创建   

***********************************************************************/


#include "Noise3D.h"

static UINT VBstride_Default = sizeof(N_DefaultVertex);		//VertexBuffer的每个元素的字节跨度
static UINT VBoffset = 0;				//VertexBuffer顶点序号偏移 因为从头开始所以offset是0

NoiseMesh::NoiseMesh()
{
	m_VertexCount	= 0;
	m_IndexCount	= 0;
	m_MaterialID_for_SetMaterial = NOISE_MACRO_DEFAULT_MATERIAL_ID;
	m_RotationX_Pitch = 0.0f;
	m_RotationY_Yaw = 0.0f;
	m_RotationZ_Roll = 0.0f;
	m_ScaleX = 1.0f;
	m_ScaleY = 1.0f;
	m_ScaleZ = 1.0f;

	m_pVertexInMem		= new std::vector<N_DefaultVertex>;
	m_pIndexInMem		= new std::vector<UINT>;
	m_pPrimitiveInfoList	= new std::vector<N_PrimitiveInfo>;//store tex/mat ID of a triangle
	m_pSubsetInfoList		= new std::vector<N_SubsetInfo>;//store [a,b] of a subset

	m_pMatrixWorld		= new NMATRIX;
	m_pMatrixWorldInvTranspose	= new NMATRIX;
	m_pPosition					= new NVECTOR3(0 ,0, 0);
	m_pBoundingBox_Min	= new NVECTOR3(0, 0, 0);
	m_pBoundingBox_Max	= new NVECTOR3(0, 0, 0);
	D3DXMatrixIdentity(m_pMatrixWorld);
	D3DXMatrixIdentity(m_pMatrixWorldInvTranspose);
};

void NoiseMesh::SelfDestruction()
{
	ReleaseCOM(m_pVertexBuffer);
	ReleaseCOM(m_pIndexBuffer);
};

void	NoiseMesh::CreatePlane(float fWidth,float fHeight,UINT iRowCount,UINT iColumnCount)
{
	//check if the input "Step Count" is illegal
	if(iColumnCount <= 2)	{iColumnCount =2;}
	if(iRowCount <= 2)		{iRowCount = 2;}

	if(m_pVertexBuffer != NULL){ReleaseCOM(m_pVertexBuffer);}
	m_pVertexInMem->clear();

	if(m_pIndexBuffer!= NULL){ReleaseCOM(m_pIndexBuffer);}
	m_pIndexInMem->clear();

	//this function can generate a Quad according to basis
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth/2,0,-fHeight/2),
		NVECTOR3(0,0,fHeight/(float)(iRowCount-1)),
		NVECTOR3(fWidth/(float)(iColumnCount-1),0,0),
		iRowCount,
		iColumnCount,0);

	
	//Prepare to update to GPU
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex,sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVertexInMem->at(0);
	m_VertexCount = m_pVertexInMem->size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index,sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIndexInMem->at(0);
	m_IndexCount = m_pIndexInMem->size();

	//最后
	mFunction_CreateGpuBuffers( &tmpInitData_Vertex ,m_VertexCount,&tmpInitData_Index,m_IndexCount);

	//user-set material
	SetMaterial(m_MaterialID_for_SetMaterial);
};

void NoiseMesh::CreateBox(float fWidth,float fHeight,float fDepth,UINT iDepthStep,UINT iWidthStep,UINT iHeightStep)
{
	//If the user has create sth before,then we will destroy the former
	//VB in order to create a new size buffer
	if(m_pVertexBuffer!= NULL){ReleaseCOM(m_pVertexBuffer);}
	m_pVertexInMem->clear();

	if(m_pIndexBuffer!= NULL){ReleaseCOM(m_pIndexBuffer);}
	m_pIndexInMem->clear();

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
	float tmpStep1 = fWidth/(float)(iWidthStep-1);
	float tmpStep2 = fDepth/(float)(iDepthStep-1)	;
	tmpBaseIndex = 0;
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth/2,		-fHeight/2,		-fDepth/2),
		NVECTOR3(tmpStep1,0,0),
		NVECTOR3(0,0,	tmpStep2),
		iWidthStep,
		iDepthStep,
		tmpBaseIndex);

	//TOP- NORMAL√
	tmpStep1 = fDepth/(float)(iDepthStep-1)	;
	tmpStep2 = fWidth/(float)(iWidthStep-1);
	tmpBaseIndex = m_pVertexInMem->size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth/2,		fHeight/2,		-fDepth/2),
		NVECTOR3(0,0,tmpStep1),
		NVECTOR3(tmpStep2,0,0),
		iDepthStep,
		iWidthStep,
		tmpBaseIndex);

	//LEFT- NORMAL√
	tmpStep1 = fDepth/(float)(iDepthStep-1)	;
	tmpStep2 = fHeight/(float)(iHeightStep-1);
	tmpBaseIndex = m_pVertexInMem->size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth/2,	-fHeight/2,	-fDepth/2),
		NVECTOR3(0,0,tmpStep1),
		NVECTOR3(0,tmpStep2,0),
		iDepthStep,
		iHeightStep,
		tmpBaseIndex);

	//RIGHT- NORMAL √
	tmpStep1 = fHeight/(float)(iHeightStep-1);
	tmpStep2 = fDepth/(float)(iDepthStep-1)	;
	tmpBaseIndex = m_pVertexInMem->size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth/2,	-fHeight/2,	-fDepth/2),
		NVECTOR3(0,tmpStep1,0),
		NVECTOR3(0,0,tmpStep2),
		iHeightStep,
		iDepthStep,
		tmpBaseIndex);


	//FRONT- NORMAL√
	tmpStep1 = fHeight/(float)(iHeightStep-1);
	tmpStep2 = fWidth/(float)(iWidthStep-1)	;
	tmpBaseIndex = m_pVertexInMem->size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth/2,	-fHeight/2,	-fDepth/2),
		NVECTOR3(0,tmpStep1,0),
		NVECTOR3(tmpStep2,0,0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex);

	//BACK- NORMAL √
	tmpStep1 = fHeight/(float)(iHeightStep-1);
	tmpStep2 = -fWidth/(float)(iWidthStep-1)	;
	tmpBaseIndex = m_pVertexInMem->size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth/2,	-fHeight/2,	fDepth/2),
		NVECTOR3(0,tmpStep1,0),
		NVECTOR3(tmpStep2,0,0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex);

	//Prepare to Create Gpu Buffers
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex,sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVertexInMem->at(0);
	m_VertexCount = m_pVertexInMem->size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index,sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIndexInMem->at(0);
	m_IndexCount = m_pIndexInMem->size();

	//transmit to gpu
	mFunction_CreateGpuBuffers( &tmpInitData_Vertex ,m_VertexCount,&tmpInitData_Index,m_IndexCount);

	//user-set material
	SetMaterial(m_MaterialID_for_SetMaterial);
}

void	NoiseMesh::CreateSphere(float fRadius,UINT iColumnCount, UINT iRingCount)
{
	//check if the input "Step Count" is illegal
	if(iColumnCount <= 3)	{iColumnCount =3;}
	if(iRingCount <= 1)		{iRingCount = 1;}

	if(m_pVertexBuffer != NULL){ReleaseCOM(m_pVertexBuffer);}
	m_pVertexInMem->clear();

	if(m_pIndexBuffer!= NULL){ReleaseCOM(m_pIndexBuffer);}
	m_pIndexInMem->clear();

	#pragma region GenerateVertex

	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be restored in the last 2 position in this array
	//the first column will be duplicated to achieve adequate texture mapping
	NVECTOR3* tmpV;
	NVECTOR2* tmpTexCoord;
	UINT tmpVertexCount = (iColumnCount+1) * iRingCount +2;
	tmpV			  = new NVECTOR3[tmpVertexCount];
	tmpTexCoord = new NVECTOR2[tmpVertexCount];
	tmpV[tmpVertexCount-2] = NVECTOR3(NVECTOR3(0,fRadius,0));			//TOP vertex
	tmpV[tmpVertexCount-1] = NVECTOR3(NVECTOR3(0,-fRadius,0));		//BOTTOM vertex
	tmpTexCoord[tmpVertexCount-2] = NVECTOR2(0.5f,0);			//TOP vertex
	tmpTexCoord[tmpVertexCount-1] = NVECTOR2(0.5f,1.0f);			//BOTTOM vertex





	//i,j will be used for iterating , and k will be the subscript
	UINT 	i=0,j=0, k=0;
	float	tmpX,tmpY,tmpZ,tmpRingRadius;


	//Calculate the Step length (步长)
	float	StepLength_AngleY =		MATH_PI / (iRingCount +1); // distances between each level (ring)
	float StepLength_AngleXZ =		2*MATH_PI / iColumnCount;


	//start to iterate
	for(i = 0;i < iRingCount ;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for( j = 0; j <	iColumnCount+1 ; j++)
		{
			//the Y coord of  current ring 
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

			k++;
		}
	}


	//add to Memory
	N_DefaultVertex tmpCompleteV;
	for(i =0;i<tmpVertexCount;i++)
	{
		tmpCompleteV.Pos		= tmpV[i];
		tmpCompleteV.Normal = NVECTOR3(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius);
		tmpCompleteV.Color	= 	NVECTOR4(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius,1.0f);
		tmpCompleteV.TexCoord = tmpTexCoord[i];
		m_pVertexInMem->push_back(tmpCompleteV);
	}

	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex,sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVertexInMem->at(0);
	m_VertexCount = tmpVertexCount;

	#pragma endregion GenerateVertex
	
	#pragma region GenerateIndex

	//Generate Indices of a ball
	//deal with the middle
	//every Ring grows a triangle net with lower level ring
	for( i=0; i<iRingCount-1; i++)
	{
		for( j=0; j<iColumnCount; j++)
		{
			/*	
					k	_____ k+1
						|    /
						|  /
						|/		k+2
		
			*/
			//+1是因为复制了第一列，比原来设好的列数多出一列
			m_pIndexInMem->push_back(	i*			(iColumnCount+1)		+j		+0);
			m_pIndexInMem->push_back(	i*			(iColumnCount+1)		+j		+1);
			m_pIndexInMem->push_back(	(i+1)*	(iColumnCount	+1)		+j		+0);

			/*
						k+3
					    /|
					  /  |
			k+5	/___|	k+4

			*/
			m_pIndexInMem->push_back(	i*			(iColumnCount+1)		+j		+1);
			m_pIndexInMem->push_back(	(i+1)*	(iColumnCount+1)		+j		+1);
			m_pIndexInMem->push_back(	(i+1)*	(iColumnCount+1)		+j		+0);
			
		}
	}


	//deal with the TOP/BOTTOM
	
	for(j =0;j<iColumnCount;j++)
	{
		m_pIndexInMem->push_back(j+1);
		m_pIndexInMem->push_back(j) ;
		m_pIndexInMem->push_back(tmpVertexCount-2);	//index of top vertex

		m_pIndexInMem->push_back((iColumnCount+1)* (iRingCount-1) + j);
		m_pIndexInMem->push_back((iColumnCount+1) * (iRingCount-1) + j+1);
		m_pIndexInMem->push_back(tmpVertexCount -1); //index of bottom vertex
	}
	

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index,sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIndexInMem->at(0);
	//a single Triangle
	m_IndexCount = m_pIndexInMem->size();//(iColumnCount+1) * iRingCount * 2 *3

	#pragma endregion GenerateIndex


	//最后
	mFunction_CreateGpuBuffers( &tmpInitData_Vertex ,m_VertexCount,&tmpInitData_Index,m_IndexCount);
	//user-set material
	SetMaterial(m_MaterialID_for_SetMaterial);

};

void NoiseMesh::CreateCylinder(float fRadius,float fHeight,UINT iColumnCount,UINT iRingCount)
{
		//check if the input "Step Count" is illegal
	if(iColumnCount <= 3)	{iColumnCount =3;}
	if(iRingCount <= 2)		{iRingCount = 2;}

	if(m_pVertexBuffer != NULL){ReleaseCOM(m_pVertexBuffer);}
	m_pVertexInMem->clear();

	if(m_pIndexBuffer!= NULL){ReleaseCOM(m_pIndexBuffer);}
	m_pIndexInMem->clear();

	#pragma region GenerateVertex

	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the last "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be restored in the last 2 position in this array
	//the first column will be duplicated to achieve adequate texture mapping
	NVECTOR3* tmpV;
	NVECTOR2* tmpTexCoord;
	UINT tmpVertexCount = (iColumnCount+1) * (iRingCount+2) +2;
	tmpV				= new NVECTOR3[tmpVertexCount];
	tmpTexCoord	= new NVECTOR2[tmpVertexCount];
	tmpV[tmpVertexCount-2] = NVECTOR3(NVECTOR3(0,fHeight/2,0));		//TOP vertex
	tmpV[tmpVertexCount-1] = NVECTOR3(NVECTOR3(0,-fHeight/2,0));		//BOTTOM vertex
	tmpTexCoord[tmpVertexCount-2] = NVECTOR2(0.5f,0);			//TOP vertex
	tmpTexCoord[tmpVertexCount-1] = NVECTOR2(0.5f,1.0f);			//BOTTOM vertex




	//i,j will be used for iterating , and k will be the subscript
	UINT 	i=0,j=0, k=0;
	float	tmpX,tmpY,tmpZ;

	//Calculate the Step length (步长)
	//the RINGS include "the top ring" and "the bottom ring"
	float	StepLength_Y =			fHeight / (iRingCount-1); // distances between each level (ring)
	float StepLength_Angle =		2*MATH_PI / iColumnCount;


	//start to iterate
	for(i = 0;i < iRingCount ;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for( j = 0; j <	iColumnCount+1 ; j++)
		{
			tmpY = (fHeight/2) - i *StepLength_Y;	
			tmpX = fRadius * cos( j*StepLength_Angle);
			tmpZ = fRadius * sin( j*StepLength_Angle);
			tmpV[k] = NVECTOR3(tmpX,tmpY,tmpZ);

			//TexCoord generation, look for more detail in tech doc
			tmpTexCoord[k] = NVECTOR2((float) j/(iColumnCount-1), tmpY/ (fRadius*2+fHeight) );

			k++;
		}
	}

	//要增加TOP/BOTTOM两个RING，因为NORMAL不一样
	for( j = 0; j <	iColumnCount+1 ; j++)
	{
		tmpY = (fHeight/2);	
		tmpX = fRadius * cos( j*StepLength_Angle);
		tmpZ = fRadius * sin( j*StepLength_Angle);
		tmpV[k] = NVECTOR3(tmpX,tmpY,tmpZ);
		tmpTexCoord[k] = NVECTOR2((float)j/(iColumnCount-1), tmpY/ (fRadius*2+fHeight) );
		k++;
	}
	for( j = 0; j <	iColumnCount+1 ; j++)
	{
		tmpY = (-fHeight/2);	
		tmpX = fRadius * cos( j*StepLength_Angle);
		tmpZ = fRadius * sin( j*StepLength_Angle);
		tmpV[k] = NVECTOR3(tmpX,tmpY,tmpZ);
		tmpTexCoord[k] = NVECTOR2((float)j/(iColumnCount-1), tmpY/ (fRadius*2+fHeight) );
		k++;
	}




	//侧面(side Face) along with their normals
	N_DefaultVertex tmpCompleteV;
	for(i =0;i<(iColumnCount+1)*iRingCount;i++)
	{
		tmpCompleteV.Pos = tmpV[i];
		tmpCompleteV.Normal =  NVECTOR3(tmpV[i].x/fRadius,0,tmpV[i].z/fRadius);
		tmpCompleteV.Color =NVECTOR4(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius,1.0f);
		tmpCompleteV.TexCoord = tmpTexCoord[i];
		m_pVertexInMem->push_back(tmpCompleteV);
	}
	//TOP/BOTTOM face along with their normals
	for(i =(iColumnCount+1)*iRingCount;i<(iColumnCount+1)*(iRingCount+2);i++)
	{
		tmpCompleteV.Pos = tmpV[i];

		//set the normal according the sign of Y coord
		tmpCompleteV.Normal =  NVECTOR3(0,(tmpV[i].y>0?1.0f:-1.0f) ,0);
		tmpCompleteV.Color =NVECTOR4(tmpV[i].x/fRadius,tmpV[i].y/fRadius,tmpV[i].z/fRadius,1.0f);
		tmpCompleteV.TexCoord = tmpTexCoord[i];
		m_pVertexInMem->push_back(tmpCompleteV);
	}



	//TOP/BOTTOM Vertex
	tmpCompleteV.Pos =tmpV[tmpVertexCount-2];
	tmpCompleteV.Normal = NVECTOR3(0,1.0f,0);
	tmpCompleteV.Color    =NVECTOR4(1.0f,1.0f,1.0f,1.0f);
	tmpCompleteV.TexCoord = tmpTexCoord[tmpVertexCount-2];
	m_pVertexInMem->push_back(tmpCompleteV);

	tmpCompleteV.Pos =tmpV[tmpVertexCount-1];
	tmpCompleteV.Normal = NVECTOR3(0,-1.0f,0);
	tmpCompleteV.Color    =NVECTOR4(1.0f,1.0f,1.0f,1.0f);
	tmpCompleteV.TexCoord = tmpTexCoord[tmpVertexCount-1];
	m_pVertexInMem->push_back(tmpCompleteV);

	//,........
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex,sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVertexInMem->at(0);
	m_VertexCount = m_pVertexInMem->size();//tmpVertexCount;

	#pragma endregion GenerateVertex
	

	#pragma region GenerateIndex

	//Generate Indices of a ball
	//deal with the middle
	//every Ring grows a triangle net with lower level ring
	for( i=0; i<iRingCount-1; i++)
	{
		for( j=0; j<iColumnCount; j++)
		{
			
			/*	
					k	_____ k+1
						|    /
						|  /
						|/		k+2
		
			*/
			m_pIndexInMem->push_back(	i*			(iColumnCount+1)		+j		+0);
			m_pIndexInMem->push_back(	i*			(iColumnCount+1)		+j		+1);
			m_pIndexInMem->push_back(	(i+1)*	(iColumnCount+1)		+j		+0);

			/*
						k+3
					    /|
					  /  |
			k+5	/___|	k+4

			*/
			m_pIndexInMem->push_back(	i*			(iColumnCount+1)		+j		+1);
			m_pIndexInMem->push_back(	(i+1)*	(iColumnCount+1)		+j		+1);
			m_pIndexInMem->push_back(	(i+1)*	(iColumnCount+1)		+j		+0);
		}
	}


	//deal with the TOP/BOTTOM
	for( j =0;j<iColumnCount;j++)
	{
		m_pIndexInMem->push_back((iColumnCount+1)*iRingCount+j);
		m_pIndexInMem->push_back((iColumnCount+1)*iRingCount+j+1) ;
		m_pIndexInMem->push_back(tmpVertexCount-2);	//index of top vertex

		m_pIndexInMem->push_back((iColumnCount+1) * (iRingCount+1) + j);
		m_pIndexInMem->push_back((iColumnCount+1) * (iRingCount+1) + j +1);
		m_pIndexInMem->push_back(tmpVertexCount -1); //index of bottom vertex

	}


	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index,sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIndexInMem->at(0);
	//a single Triangle
	m_IndexCount = m_pIndexInMem->size();//iColumnCount * iRingCount * 2 *3

	#pragma endregion GenerateIndex


	//最后
	mFunction_CreateGpuBuffers( &tmpInitData_Vertex ,m_VertexCount,&tmpInitData_Index,m_IndexCount);
	//user-set material
	SetMaterial(m_MaterialID_for_SetMaterial);
};

BOOL NoiseMesh::LoadFile_STL(char * pFilePath)
{
	//check if buffers have been created
	if (m_pVertexBuffer != NULL) { ReleaseCOM(m_pVertexBuffer); }
	m_pVertexInMem->clear();

	if (m_pIndexBuffer != NULL) { ReleaseCOM(m_pIndexBuffer); }
	m_pIndexInMem->clear();


	std::vector<NVECTOR3> tmpVertexList;
	std::vector<NVECTOR3> tmpNormalList;
	std::vector<char>			   tmpInfo;
	N_DefaultVertex	tmpCompleteV;
	NVECTOR3			tmpBoundingBoxCenter(0, 0, 0);

	//加载STL
	NoiseFileManager::ImportFile_STL(pFilePath, &tmpVertexList, m_pIndexInMem, &tmpNormalList, &tmpInfo);

	//先计算包围盒，就能求出网格的中心点（不一定是Mesh Space的原点）
	mFunction_ComputeBoundingBox(&tmpVertexList);

	//计算包围盒中心点
	tmpBoundingBoxCenter = NVECTOR3(
		(m_pBoundingBox_Max->x + m_pBoundingBox_Min->x) / 2.0f,
		(m_pBoundingBox_Max->y + m_pBoundingBox_Min->y) / 2.0f,
		(m_pBoundingBox_Max->z + m_pBoundingBox_Min->z) / 2.0f);



	UINT i = 0;UINT k = 0;
	for (i = 0;i < tmpVertexList.size();i++)
	{
		tmpCompleteV.Color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		tmpCompleteV.Pos = tmpVertexList.at(i);
		tmpCompleteV.Normal = tmpNormalList.at(k);
		tmpCompleteV.TexCoord = mFunction_ComputeTexCoord_SphericalWrap(tmpBoundingBoxCenter, tmpCompleteV.Pos);
		m_pVertexInMem->push_back(tmpCompleteV);

		//每新增了一个三角形3个顶点 就要轮到下个三角形的法线了
		if (i % 3 == 2) { k++; }
	}


	//Prepare to update to GPU
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVertexInMem->at(0);
	m_VertexCount = m_pVertexInMem->size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIndexInMem->at(0);
	m_IndexCount = m_pIndexInMem->size();

	//最后
	mFunction_CreateGpuBuffers(&tmpInitData_Vertex, m_VertexCount, &tmpInitData_Index, m_IndexCount);

	//user-set material
	SetMaterial(m_MaterialID_for_SetMaterial);

	return FALSE;
}

void	NoiseMesh::SetMaterial(UINT matID)
{
	//if mat ID is valid
	if (matID < m_pFatherScene->m_pChildMaterialMgr->GetMaterialCount())
	{

		//this is just for user-set ;;;  In File-Loading method,we will use material settings in file
		m_MaterialID_for_SetMaterial = matID;
		
		//actually in this function filling m_pPrimitiveInfoList is optional .(I dont want it to be empty= =)
		//but when loading mesh from file, this primitive info  list should be filled to be sorted.
		for (UINT i = 0;	i < m_pIndexInMem->size()/3	;i ++)
		{
			N_PrimitiveInfo tmpInfo;
			m_pPrimitiveInfoList->push_back(tmpInfo);
			m_pPrimitiveInfoList->at(i).index1 = m_pIndexInMem->at(3 * i);
			m_pPrimitiveInfoList->at(i).index2 = m_pIndexInMem->at(3 * i + 1);
			m_pPrimitiveInfoList->at(i).index3 = m_pIndexInMem->at(3 * i + 2);
			m_pPrimitiveInfoList->at(i).mMatID = matID;
		}

		//but set the "SUBSET" list to a correct state is crucial.
		N_SubsetInfo tmpSubset;
		tmpSubset.startPrimitiveID = 0;
		tmpSubset.endPrimitiveID = m_pPrimitiveInfoList->size()-1;//count of triangles
		tmpSubset.matID = matID;

		//because this SetMaterial aim to the entire mesh (all primitives) ,s
		//o previously-defined material will be wiped out,and set to this material
		m_pSubsetInfoList->clear();
		m_pSubsetInfoList->push_back(tmpSubset);
	}
}

void NoiseMesh::SetMaterial(std::string matName)
{
	UINT matID = 0;
	matID = m_pFatherScene->m_pChildMaterialMgr->GetIndexByName(&matName);
	//if matName is valid , matID will not equal to 0
	SetMaterial(matID);
}

BOOL NoiseMesh::AddToRenderList()
{
	if(m_pFatherScene == NULL)
	{
		assert (FALSE:"NoiseScene Has Not Been Created!");
		return FALSE;
	}
	//scene是它的友元类；往Scene里管理的RenderList加上自己的指针
	m_pFatherScene->m_pChildRenderer->m_pRenderList_Mesh->push_back(this);
	return TRUE;
};

void NoiseMesh::SetPosition(float x,float y,float z)
{
	m_pPosition->x =x;
	m_pPosition->y =y;
	m_pPosition->z =z;
}

void NoiseMesh::SetRotation(float angleX, float angleY, float angleZ)
{
	m_RotationX_Pitch	= angleX;
	m_RotationY_Yaw		= angleY;
	m_RotationZ_Roll		= angleZ;
}

void NoiseMesh::SetRotationX_Pitch(float angleX)
{
	m_RotationX_Pitch = angleX;
};

void NoiseMesh::SetRotationY_Yaw(float angleY)
{
	m_RotationY_Yaw = angleY;
};

void NoiseMesh::SetRotationZ_Roll(float angleZ)
{
	m_RotationZ_Roll = angleZ;
}

void NoiseMesh::SetScale(float scaleX, float scaleY, float scaleZ)
{
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
	m_ScaleZ = scaleZ;
}

void NoiseMesh::SetScaleX(float scaleX)
{
	m_ScaleX = scaleX;
}

void NoiseMesh::SetScaleY(float scaleY)
{
	m_ScaleY = scaleY;
}

void NoiseMesh::SetScaleZ(float scaleZ)
{
	m_ScaleZ = scaleZ;
}

UINT NoiseMesh::GetVertexCount()
{
	return m_pVertexInMem->size();
}

void NoiseMesh::GetVertex(UINT iIndex, N_DefaultVertex& outVertex)
{
	if (iIndex < m_pVertexInMem->size())
	{
		outVertex = m_pVertexInMem->at(iIndex);
	}
}

void NoiseMesh::GetVertexBuffer(std::vector<N_DefaultVertex>& outBuff)
{
	std::vector<N_DefaultVertex>::iterator iterBegin, iterLast;
	iterBegin = m_pVertexInMem->begin();
	iterLast = m_pVertexInMem->end();
	outBuff.assign(iterBegin,iterLast);
}

NVECTOR3 NoiseMesh::GetBoundingBoxMax()
{
	if (*m_pBoundingBox_Max == *m_pBoundingBox_Min)
	{
		mFunction_ComputeBoundingBox();
	}

	return *m_pBoundingBox_Max;
};

NVECTOR3 NoiseMesh::GetBoundingBoxMin()
{
	if (*m_pBoundingBox_Max == *m_pBoundingBox_Min)
	{
		mFunction_ComputeBoundingBox();
	}

	return *m_pBoundingBox_Min;
}



/***********************************************************************
								PRIVATE					                    
***********************************************************************/

BOOL NoiseMesh::mFunction_CreateGpuBuffers
	(	D3D11_SUBRESOURCE_DATA* pVertexDataInMem,int iVertexCount,D3D11_SUBRESOURCE_DATA* pIndexDataInMem,int iIndexCount)
{
	//Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth =  sizeof(N_DefaultVertex)* iVertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	vbd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	vbd.StructureByteStride = 0;

	//Create Buffers
	int hr =0;
	hr = g_pd3dDevice->CreateBuffer(&vbd,pVertexDataInMem,&m_pVertexBuffer);
	HR_DEBUG(hr,"VERTEX BUFFER创建失败");


	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(int) * iIndexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	ibd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	ibd.StructureByteStride = 0;

	//Create Buffers
	hr = g_pd3dDevice->CreateBuffer(&ibd,pIndexDataInMem,&m_pIndexBuffer);
	HR_DEBUG(hr,"INDEX BUFFER创建失败");

	return TRUE;
}
	
void	NoiseMesh::mFunction_Build_A_Quad
	(NVECTOR3 vOriginPoint,NVECTOR3 vBasisVector1,NVECTOR3 vBasisVector2,UINT StepCount1,UINT StepCount2,UINT iBaseIndex)
{

	#pragma region GenerateVertex
	UINT i=0,j=0;
	NVECTOR3 tmpNormal;
	N_DefaultVertex tmpCompleteV;
	D3DXVec3Cross(&tmpNormal,&vBasisVector1,&vBasisVector2);
	D3DXVec3Normalize(&tmpNormal,&tmpNormal);
	for(i=0;i<StepCount1;i++ )
		for(j=0;j<StepCount2;j++)
		{
				tmpCompleteV.Normal = tmpNormal;
				tmpCompleteV.Pos		= NVECTOR3(vOriginPoint+(float)i*vBasisVector1+(float)j*vBasisVector2);
				tmpCompleteV.Color	= NVECTOR4(((float)i/StepCount1),((float)j/StepCount2),0.5f,1.0f);
				tmpCompleteV.TexCoord=NVECTOR2( (float)i/(StepCount1-1) , ( (float)j/StepCount2));
				m_pVertexInMem->push_back(tmpCompleteV);
		}

	#pragma endregion GenerateVertex


	#pragma region GenerateIndex
		i=0;j=0;
	for(i=0;i<StepCount1-1;i++)
	{
		for(j=0;j<StepCount2-1;j++)
		{
			m_pIndexInMem->push_back(iBaseIndex+i *		StepCount2 + j		);
			m_pIndexInMem->push_back(iBaseIndex+i *		StepCount2 + j +1);
			m_pIndexInMem->push_back(iBaseIndex+(i+1)* StepCount2 + j		);

			m_pIndexInMem->push_back(iBaseIndex+i *		StepCount2 + j +1);
			m_pIndexInMem->push_back(iBaseIndex+(i+1) *StepCount2 + j		);
			m_pIndexInMem->push_back(iBaseIndex+(i+1)* StepCount2 + j+1	);
		}
	}

	#pragma endregion GenerateIndex

};

void	NoiseMesh::mFunction_UpdateWorldMatrix()
{

	D3DXMATRIX	tmpMatrixScaling;
	D3DXMATRIX	tmpMatrixTranslation;
	D3DXMATRIX	tmpMatrixRotation;
	D3DXMATRIX	tmpMatrix;
	float tmpDeterminant;

	//初始化输出矩阵
	D3DXMatrixIdentity(&tmpMatrix);
		
	//缩放矩阵
	D3DXMatrixScaling(&tmpMatrixScaling, m_ScaleX, m_ScaleY, m_ScaleZ);

	//旋转矩阵(连mesh也用航向角hhhhhh）
	D3DXMatrixRotationYawPitchRoll(&tmpMatrixRotation, m_RotationY_Yaw, m_RotationX_Pitch, m_RotationZ_Roll);

	//平移矩阵
	D3DXMatrixTranslation(&tmpMatrixTranslation, m_pPosition->x, m_pPosition->y, m_pPosition->z);

	//先缩放，再旋转，再平移（跟viewMatrix有点区别）
	D3DXMatrixMultiply(&tmpMatrix, &tmpMatrix, &tmpMatrixScaling);
	D3DXMatrixMultiply(&tmpMatrix,&tmpMatrix,&tmpMatrixRotation);
	D3DXMatrixMultiply(&tmpMatrix, &tmpMatrix, &tmpMatrixTranslation);
	*m_pMatrixWorld = tmpMatrix;

	//求用于转换Normal的InvTranspose	因为要Trans 之后再来一次Trans才能更新 所以就可以省了
	D3DXMatrixInverse(m_pMatrixWorldInvTranspose,&tmpDeterminant,m_pMatrixWorld);

	//Update到GPU前要先转置
	D3DXMatrixTranspose(m_pMatrixWorld,m_pMatrixWorld);

	//WorldInvTranspose
	//D3DXMatrixTranspose(m_pMatrixWorldInvTranspose,&tmpMatrix);

}

void NoiseMesh::mFunction_ComputeBoundingBox()
{
	//计算包围盒.......重载1

	UINT i = 0;
	NVECTOR3 tmpV;
	//遍历所有顶点，算出包围盒3分量均最 小/大 的两个顶点
	for (i = 0;i < m_pVertexInMem->size();i++)
	{
		//N_DEFAULT_VERTEX
		tmpV = m_pVertexInMem->at(i).Pos;
		if (tmpV.x <( m_pBoundingBox_Min->x)) { m_pBoundingBox_Min->x = tmpV.x; }
		if (tmpV.y <(m_pBoundingBox_Min->y)) { m_pBoundingBox_Min->y = tmpV.y; }
		if (tmpV.z <(m_pBoundingBox_Min->z)) { m_pBoundingBox_Min->z = tmpV.z; }

		if (tmpV.x >(m_pBoundingBox_Max->x)) { m_pBoundingBox_Max->x = tmpV.x; }
		if (tmpV.y >(m_pBoundingBox_Max->y)) { m_pBoundingBox_Max->y = tmpV.y; }
		if (tmpV.z >(m_pBoundingBox_Max->z)) { m_pBoundingBox_Max->z = tmpV.z; }
	}

}

void NoiseMesh::mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer)
{
	//计算包围盒.......重载2

	UINT i = 0;
	NVECTOR3 tmpV;
	//遍历所有顶点，算出包围盒3分量均最 小/大 的两个顶点
	for (i = 0;i < pVertexBuffer->size();i++)
	{
		tmpV = pVertexBuffer->at(i);
		if (tmpV.x <(m_pBoundingBox_Min->x)) { m_pBoundingBox_Min->x = tmpV.x; }
		if (tmpV.y <(m_pBoundingBox_Min->y)) { m_pBoundingBox_Min->y = tmpV.y; }
		if (tmpV.z <(m_pBoundingBox_Min->z)) { m_pBoundingBox_Min->z = tmpV.z; }

		if (tmpV.x >(m_pBoundingBox_Max->x)) { m_pBoundingBox_Max->x = tmpV.x; }
		if (tmpV.y >(m_pBoundingBox_Max->y)) { m_pBoundingBox_Max->y = tmpV.y; }
		if (tmpV.z >(m_pBoundingBox_Max->z)) { m_pBoundingBox_Max->z = tmpV.z; }
	}

}

NVECTOR2 NoiseMesh::mFunction_ComputeTexCoord_SphericalWrap(NVECTOR3 vBoxCenter, NVECTOR3 vPoint)
{
	//额...这个函数做简单的纹理球形包裹

	NVECTOR2 outTexCoord(0,0);
	NVECTOR3 tmpP= vPoint - vBoxCenter;

	//投影到单位球上
	D3DXVec3Normalize(&tmpP, &tmpP);

	//反三角函数算球坐标系坐标，然后角度值映射到[0,1]
	float angleYaw = 0.0f;
	float anglePitch = 0.0f;
	float tmpLength = sqrtf(tmpP.x*tmpP.x + tmpP.z*tmpP.z);

	// [ -PI/2 , PI/2 ]
	anglePitch = atan2(tmpP.y,tmpLength);

	// [ -PI	, PI ]
	angleYaw =	atan2(tmpP.z, tmpP.x);	

	//map to [0,1]
	outTexCoord.x = (angleYaw +  MATH_PI) / (2.0f * MATH_PI);
	outTexCoord.y = (anglePitch + (MATH_PI/2.0f) ) / MATH_PI;

	return outTexCoord;
};
