
/***********************************************************************

							类：NOISE Atmosphere

							简述：天空盒天空球雾 等


************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IAtmosphere::IAtmosphere()
	: mFogEnabled(FALSE),
	mSkyDomeRadiusXZ(100),
	mSkyDomeHeight(100),
	mSkyType(NOISE_ATMOSPHERE_SKYTYPE_INVALID),
	m_pVB_Gpu_Sky (nullptr),
	m_pIB_Gpu_Sky(nullptr),
	mFogCanUpdateToGpu(FALSE),
	m_pFogColor(new NVECTOR3(1.0f, 1.0f, 1.0f)),
	mFogNear (10),
	mFogFar (100),
	m_pVB_Mem_Sky(new std::vector<N_SimpleVertex>),
	m_pIB_Mem_Sky(new std::vector<UINT>),
	m_pSkyBoxCubeTexName(new N_UID),
	m_pSkyDomeTexName(new N_UID)
{

}

IAtmosphere::~IAtmosphere()
{
	ReleaseCOM(m_pVB_Gpu_Sky);
	ReleaseCOM(m_pIB_Gpu_Sky);
	delete m_pVB_Mem_Sky;
	delete m_pIB_Mem_Sky;
	delete m_pFogColor;
}



void IAtmosphere::SetFogEnabled(BOOL isEnabled)
{
	mFogEnabled = isEnabled;
	mFogCanUpdateToGpu = TRUE;
}

void IAtmosphere::SetFogParameter(float fogNear, float fogFar, NVECTOR3 color)
{
	//perhaps i can skip checking the size comparison between NEAR & FAR

	if(fogFar > 1)mFogFar = fogFar;
	if (fogNear > 1)	mFogNear = fogNear;

	//validate distances
	if (fogNear >= fogFar)
	{
		mFogNear	= 10;
		mFogFar		= 100;
		ERROR_MSG("SetFog : fog Near/Far invalid");
	};

	//set color
	*m_pFogColor = color;

	mFogCanUpdateToGpu = TRUE;
}

BOOL IAtmosphere::CreateSkyDome(float fRadiusXZ, float fHeight, N_UID texName)
{
	//check if the input "Step Count" is illegal
	UINT iColumnCount		= 30;
	UINT iRingCount			= 25;
	UINT tmpVertexCount	= 0;
	UINT tmpIndexCount	= 0;

	//release existed gpu buffers
	if (m_pVB_Gpu_Sky != NULL) 
	{ 
		ReleaseCOM(m_pVB_Gpu_Sky);
		m_pVB_Gpu_Sky = nullptr;
		m_pVB_Mem_Sky->clear();
	}


	if (m_pIB_Gpu_Sky != NULL)
	{
		ReleaseCOM(m_pIB_Gpu_Sky);
		m_pIB_Gpu_Sky = nullptr;
		m_pIB_Mem_Sky->clear();
	}

	//set built-in var
	*m_pSkyDomeTexName = texName;
	mSkyDomeRadiusXZ = fRadiusXZ;
	mSkyDomeHeight = fHeight;

	//delegate vert/idx creation duty to MeshGenerator 
	mMeshGenerator.CreateSkyDome(fRadiusXZ, fHeight, iColumnCount, iRingCount, *m_pVB_Mem_Sky, *m_pIB_Mem_Sky);

	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &m_pVB_Mem_Sky->at(0);

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &m_pIB_Mem_Sky->at(0);


	 //Create VERTEX BUFFER (GPU)
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_SimpleVertex)* m_pVB_Mem_Sky->size();
	vbd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	vbd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	vbd.StructureByteStride = 0;
	int hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu_Sky);
	HR_DEBUG(hr, "Noise IAtmosphere : creating VERTEX BUFFER failed!!");

	//create index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(UINT) * m_pIB_Mem_Sky->size();
	ibd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	ibd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	ibd.StructureByteStride = 0;
	hr = g_pd3dDevice11->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu_Sky);
	HR_DEBUG(hr, "INDEX BUFFER创建失败");


	//set current sky type
	mSkyType = NOISE_ATMOSPHERE_SKYTYPE_DOME;

	return TRUE;
}

void		IAtmosphere::SetSkyDomeTexture(N_UID texName)
{
	//validation will be done in NoiseRenderer
	*m_pSkyDomeTexName = texName;
}

BOOL IAtmosphere::CreateSkyBox(float fWidth, float fHeight, float fDepth, N_UID cubeMapTexName)
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

	//.......
	mMeshGenerator.CreateSkyBox(fWidth, fHeight, fDepth, *m_pVB_Mem_Sky, *m_pIB_Mem_Sky);


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
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu_Sky);
	HR_DEBUG(hr, "Noise IAtmosphere : creating VERTEX BUFFER failed!!");

	//create index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(UINT) * tmpIndexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	ibd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	ibd.StructureByteStride = 0;
	hr = g_pd3dDevice11->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu_Sky);
	HR_DEBUG(hr, "INDEX BUFFER创建失败");


	//set texture ID
	*m_pSkyBoxCubeTexName = cubeMapTexName;
	mSkyBoxWidth = fWidth;
	mSkyBoxHeight = fHeight;
	mSkyBoxDepth = fDepth;
	//set current sky type
	mSkyType = NOISE_ATMOSPHERE_SKYTYPE_BOX;

	return TRUE;
}

void IAtmosphere::SetSkyBoxTexture(std::string cubeMapTexName)
{
	*m_pSkyBoxCubeTexName = cubeMapTexName;
}


/***************************************************************
									PRIVATE
*****************************************************************/
