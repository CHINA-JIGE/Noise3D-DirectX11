
/***********************************************************************

							类：NOISE Atmosphere

							简述：天空盒天空球雾 等


************************************************************************/

#include "Noise3D.h"

NoiseAtmosphere::NoiseAtmosphere()
{
	mFogEnabled = FALSE;
	mFogCanUpdateToGpu = FALSE;
	m_pFogColor = new NVECTOR3(1.0f, 1.0f, 1.0f);
	mFogNear = 10;
	mFogFar = 100;
	m_pVB_Mem_Sky	= new std::vector<N_SimpleVertex>;
	m_pIB_Mem_Sky = new std::vector<UINT>;
	m_pVB_Gpu_Sky	= nullptr;
	m_pIB_Gpu_Sky		= nullptr;
	mSkyDomeRadiusXZ =100;
	mSkyDomeHeight = 100;
	mSkyDomeTextureID = NOISE_MACRO_INVALID_TEXTURE_ID;
	mSkyType = NOISE_ATMOSPHERE_SKYTYPE_INVALID;

}

void NoiseAtmosphere::SelfDestruction()
{

};

BOOL NoiseAtmosphere::AddToRenderList()
{
	if (m_pFatherScene == NULL)
	{
		DEBUG_MSG1("NoiseScene Has Not Been Created!");
		return FALSE;
	}
	
	//scene是它的友元类；往Scene里管理的RenderList加上自己的指针
	m_pFatherScene->m_pChildRenderer->m_pRenderList_Atmosphere->push_back(this);

	//this sentence is to unify render command, that fog color will only be rendered after ADDTORENDERLIST();
	mFogHasBeenAddedToRenderList = TRUE;

	return TRUE;
}

void NoiseAtmosphere::SetFogEnabled(BOOL isEnabled)
{
	mFogEnabled = isEnabled;
	mFogCanUpdateToGpu = TRUE;
}

void NoiseAtmosphere::SetFogParameter(float fogNear, float fogFar, NVECTOR3 color)
{
	//perhaps i can skip checking the size comparison between NEAR & FAR

	if(fogFar > 1)mFogFar = fogFar;
	if (fogNear > 1)	mFogNear = fogNear;

	//validate distances
	if (fogNear >= fogFar)
	{
		mFogNear	= 10;
		mFogFar		= 100;
		DEBUG_MSG1("SetFog : fog Near/Far invalid");
	};

	//set color
	*m_pFogColor = color;

	mFogCanUpdateToGpu = TRUE;
}

BOOL NoiseAtmosphere::CreateSkyDome(float fRadiusXZ, float fHeight,UINT texID)
{
	//check if the input "Step Count" is illegal
	UINT iColumnCount		= 25;
	UINT iRingCount			= 30;
	UINT tmpVertexCount	= 0;
	UINT tmpIndexCount	= 0;

	//release existed gpu buffers
	if (m_pVB_Gpu_Sky != NULL) 
	{ 
		ReleaseCOM(m_pVB_Gpu_Sky);
		m_pVB_Mem_Sky->clear();
	}


	if (m_pIB_Gpu_Sky != NULL)
	{
		ReleaseCOM(m_pIB_Gpu_Sky);
		m_pIB_Mem_Sky->clear();
	}

	//set built-in var
	mSkyDomeTextureID = texID;
	mSkyDomeRadiusXZ = fRadiusXZ;
	mSkyDomeHeight = fHeight;


#pragma region GenerateVertex

	//iColunmCount : Slices of Columns (Cut up the ball Vertically)
	//iRingCount: Slices of Horizontal Rings (Cut up the ball Horizontally)
	//the "+2" refers to the TOP/BOTTOM vertex
	//the TOP/BOTTOM vertex will be restored in the last 2 position in this array
	//the first column will be duplicated to achieve adequate texture mapping
	NVECTOR3* tmpV;
	NVECTOR2* tmpTexCoord;
	tmpVertexCount = (iColumnCount + 1) * iRingCount + 2;
	tmpV = new NVECTOR3[tmpVertexCount];
	tmpTexCoord = new NVECTOR2[tmpVertexCount];
	tmpV[tmpVertexCount - 2] = NVECTOR3(NVECTOR3(0, fHeight, 0));			//TOP vertex
	tmpV[tmpVertexCount - 1] = NVECTOR3(NVECTOR3(0, -fHeight, 0));		//BOTTOM vertex
	tmpTexCoord[tmpVertexCount - 2] = NVECTOR2(0.5f, 0);			//TOP vertex
	tmpTexCoord[tmpVertexCount - 1] = NVECTOR2(0.5f, 1.0f);			//BOTTOM vertex

	//i,j will be used for iterating , and k will be the subscript
	UINT 	i = 0, j = 0, k = 0;
	float	tmpX, tmpY, tmpZ, tmpRingRadius;


	//Calculate the Step length (步长)
	float	StepLength_AngleY = MATH_PI / (iRingCount + 1); // distances between each level (ring)
	float StepLength_AngleXZ = 2 * MATH_PI / iColumnCount;


	//start to iterate
	for (i = 0;i < iRingCount;i++)
	{
		//Generate Vertices ring By ring ( from top to down )
		//the first column will be duplicated to achieve adequate texture mapping
		for (j = 0; j < iColumnCount + 1; j++)
		{
			//the Y coord of  current ring 
			tmpY = fHeight *sin(MATH_PI / 2 - (i + 1) *StepLength_AngleY);

			// radius of current horizontal ring 
			tmpRingRadius = fRadiusXZ* sqrtf(1 - (tmpY * tmpY)/(fHeight*fHeight));

			////trigonometric function(三角函数)
			tmpX = tmpRingRadius * cos(j*StepLength_AngleXZ);

			//...
			tmpZ = tmpRingRadius * sin(j*StepLength_AngleXZ);

			//...
			tmpV[k] = NVECTOR3(tmpX, tmpY, tmpZ);

			//map the i,j to closed interval [0,1] respectively , to proceed a spheric texture wrapping
			tmpTexCoord[k] = NVECTOR2((float)j / (iColumnCount), (float)i / (iRingCount - 1));

			k++;
		}
	}


	//add to Memory
	N_SimpleVertex tmpCompleteV;
	for (i = 0;i<tmpVertexCount;i++)
	{
		tmpCompleteV.Pos = tmpV[i];
		tmpCompleteV.Color = NVECTOR4(tmpV[i].x / fRadiusXZ, tmpV[i].y / fHeight, tmpV[i].z / fRadiusXZ, 1.0f);
		tmpCompleteV.TexCoord = tmpTexCoord[i];
		m_pVB_Mem_Sky->push_back(tmpCompleteV);
	}

	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVB_Mem_Sky->at(0);


#pragma endregion GenerateVertex

#pragma region GenerateIndex

	//Generate Indices of a ball
	//deal with the middle
	//every Ring grows a triangle net with lower level ring
	for (i = 0; i<iRingCount - 1; i++)
	{
		for (j = 0; j<iColumnCount; j++)
		{
			/*
			k	_____ k+1
			|    /
			|  /
			|/		k+2

			*/
			//rotating order is different from the one in mesh ,because sky dome face inside
			m_pIB_Mem_Sky->push_back(i*			(iColumnCount + 1) + j + 0);
			m_pIB_Mem_Sky->push_back((i + 1)*	(iColumnCount + 1) + j + 0);
			m_pIB_Mem_Sky->push_back(i*			(iColumnCount + 1) + j + 1);


			/*
			k+3
			/|
			/  |
			k+5	/___|	k+4

			*/
			m_pIB_Mem_Sky->push_back(i*			(iColumnCount + 1) + j + 1);
			m_pIB_Mem_Sky->push_back((i + 1)*	(iColumnCount + 1) + j + 0);
			m_pIB_Mem_Sky->push_back((i + 1)*	(iColumnCount + 1) + j + 1);
			//m_pIB_Mem_Sky->push_back((i + 1)*	(iColumnCount + 1) + j + 0);

		}
	}


	//deal with the TOP/BOTTOM

	for (j = 0;j<iColumnCount;j++)
	{
		m_pIB_Mem_Sky->push_back(j + 1);
		m_pIB_Mem_Sky->push_back(tmpVertexCount - 2);	//index of top vertex
		m_pIB_Mem_Sky->push_back(j);
		//m_pIB_Mem_Sky->push_back(tmpVertexCount - 2);	//index of top vertex

		m_pIB_Mem_Sky->push_back((iColumnCount + 1)* (iRingCount - 1) + j);
		m_pIB_Mem_Sky->push_back(tmpVertexCount - 1); //index of bottom vertex
		m_pIB_Mem_Sky->push_back((iColumnCount + 1) * (iRingCount - 1) + j + 1);
		//m_pIB_Mem_Sky->push_back(tmpVertexCount - 1); //index of bottom vertex
	}


	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIB_Mem_Sky->at(0);
	//a single Triangle
	tmpIndexCount = m_pIB_Mem_Sky->size();//(iColumnCount+1) * iRingCount * 2 *3

#pragma endregion GenerateIndex



	 //Create VERTEX BUFFER (GPU)
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_SimpleVertex)* tmpVertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	vbd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	vbd.StructureByteStride = 0;
	int hr = 0;
	hr = g_pd3dDevice->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu_Sky);
	HR_DEBUG(hr, "Noise Atmosphere : creating VERTEX BUFFER failed!!");

	//create index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(UINT) * tmpIndexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	ibd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	ibd.StructureByteStride = 0;
	hr = g_pd3dDevice->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu_Sky);
	HR_DEBUG(hr, "INDEX BUFFER创建失败");

	//set current sky type
	mSkyType = NOISE_ATMOSPHERE_SKYTYPE_DOME;

	return TRUE;
}

void		NoiseAtmosphere::SetSkyDomeTexture(UINT texID)
{
	//validation will be done in NoiseRenderer
	mSkyDomeTextureID = texID;
};

BOOL NoiseAtmosphere::CreateSkyBox(float fWidth, float fHeight, float fDepth,UINT cubeMapTexID)
{

	UINT tmpVertexCount = 0, tmpIndexCount = 0;

	//check input size
	if (fWidth <= 0)fWidth = 1.0f;
	if (fHeight <= 0)fHeight = 1.0f;
	if (fDepth <= 0)fDepth = 1.0f;

	//release existed gpu buffers
	if (m_pVB_Gpu_Sky != NULL)
	{
		ReleaseCOM(m_pVB_Gpu_Sky);
		m_pVB_Mem_Sky->clear();
	}


	if (m_pIB_Gpu_Sky != NULL)
	{
		ReleaseCOM(m_pIB_Gpu_Sky);
		m_pIB_Mem_Sky->clear();
	}

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
		tmpBaseIndex);

	//TOP- NORMAL√
	tmpStep1 = fDepth / (float)(iDepthStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = m_pVB_Mem_Sky->size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, fHeight / 2, -fDepth / 2),
		NVECTOR3(0, 0, tmpStep1),
		NVECTOR3(tmpStep2, 0, 0),
		iDepthStep,
		iWidthStep,
		tmpBaseIndex);

	//LEFT- NORMAL√
	tmpStep1 = fDepth / (float)(iDepthStep - 1);
	tmpStep2 = fHeight / (float)(iHeightStep - 1);
	tmpBaseIndex = m_pVB_Mem_Sky->size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, 0, tmpStep1),
		NVECTOR3(0, tmpStep2, 0),
		iDepthStep,
		iHeightStep,
		tmpBaseIndex);

	//RIGHT- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fDepth / (float)(iDepthStep - 1);
	tmpBaseIndex = m_pVB_Mem_Sky->size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(0, 0, tmpStep2),
		iHeightStep,
		iDepthStep,
		tmpBaseIndex);


	//FRONT- NORMAL√
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = m_pVB_Mem_Sky->size();
	mFunction_Build_A_Quad(
		NVECTOR3(-fWidth / 2, -fHeight / 2, -fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex);

	//BACK- NORMAL √
	tmpStep1 = fHeight / (float)(iHeightStep - 1);
	tmpStep2 = -fWidth / (float)(iWidthStep - 1);
	tmpBaseIndex = m_pVB_Mem_Sky->size();
	mFunction_Build_A_Quad(
		NVECTOR3(fWidth / 2, -fHeight / 2, fDepth / 2),
		NVECTOR3(0, tmpStep1, 0),
		NVECTOR3(tmpStep2, 0, 0),
		iHeightStep,
		iWidthStep,
		tmpBaseIndex);



	//prepare to update to GPU
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVB_Mem_Sky->at(0);
	tmpVertexCount = m_pVB_Mem_Sky->size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIB_Mem_Sky->at(0);
	tmpIndexCount = m_pIB_Mem_Sky->size();//(iColumnCount+1) * iRingCount * 2 *3

	 //Create VERTEX BUFFER (GPU)
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_SimpleVertex)* tmpVertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	vbd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	vbd.StructureByteStride = 0;
	int hr = 0;
	hr = g_pd3dDevice->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu_Sky);
	HR_DEBUG(hr, "Noise Atmosphere : creating VERTEX BUFFER failed!!");

	//create index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(UINT) * tmpIndexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	ibd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	ibd.StructureByteStride = 0;
	hr = g_pd3dDevice->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu_Sky);
	HR_DEBUG(hr, "INDEX BUFFER创建失败");


	//set texture ID
	mSkyBoxCubeTextureID = cubeMapTexID;
	mSkyBoxWidth = fWidth;
	mSkyBoxHeight = fHeight;
	mSkyBoxDepth = fDepth;
	//set current sky type
	mSkyType = NOISE_ATMOSPHERE_SKYTYPE_BOX;

	return TRUE;
}

void NoiseAtmosphere::SetSkyBoxTexture(UINT cubeMapTexID)
{
	mSkyBoxCubeTextureID = cubeMapTexID;
};




/***************************************************************
									PRIVATE
*****************************************************************/

void	NoiseAtmosphere::mFunction_Build_A_Quad
(NVECTOR3 vOriginPoint, NVECTOR3 vBasisVector1, NVECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex)
{
	// it is used to build a Quad , or say Rectangle . StepCount is similar to the count of sections

#pragma region GenerateVertex

	UINT i = 0, j = 0;
	NVECTOR3 tmpNormal;
	N_SimpleVertex tmpCompleteV;
	D3DXVec3Cross(&tmpNormal, &vBasisVector1, &vBasisVector2);
	D3DXVec3Normalize(&tmpNormal, &tmpNormal);

	for (i = 0;i<StepCount1;i++)
		for (j = 0;j<StepCount2;j++)
		{
			tmpCompleteV.Pos = NVECTOR3(vOriginPoint + (float)i*vBasisVector1 + (float)j*vBasisVector2);
			tmpCompleteV.Color = NVECTOR4(((float)i / StepCount1), ((float)j / StepCount2), 0.5f, 1.0f);
			tmpCompleteV.TexCoord = NVECTOR2((float)i / (StepCount1 - 1), ((float)j / StepCount2));
			m_pVB_Mem_Sky->push_back(tmpCompleteV);
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
			m_pIB_Mem_Sky->push_back(iBaseIndex + i *		StepCount2 + j);
			m_pIB_Mem_Sky->push_back(iBaseIndex + (i + 1)* StepCount2 + j);
			m_pIB_Mem_Sky->push_back(iBaseIndex + i *		StepCount2 + j + 1);

			m_pIB_Mem_Sky->push_back(iBaseIndex + i *		StepCount2 + j + 1);
			m_pIB_Mem_Sky->push_back(iBaseIndex + (i + 1) *StepCount2 + j);
			m_pIB_Mem_Sky->push_back(iBaseIndex + (i + 1)* StepCount2 + j + 1);
		}
	}

#pragma endregion GenerateIndex

};



