
/***********************************************************************

							类：NOISE Atmosphere

							简述：天空盒天空球雾 等


************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

IAtmosphere::IAtmosphere()
	: mFogEnabled(false),
	mSkyDomeRadiusXZ(100),
	mSkyDomeHeight(100),
	mSkyType(NOISE_ATMOSPHERE_SKYTYPE_INVALID),
	m_pVB_Gpu (nullptr),
	m_pIB_Gpu(nullptr),
	mFogCanUpdateToGpu(false),
	mFogColor(1.0f, 1.0f, 1.0f),
	mFogNear (10),
	mFogFar (100),
	mSkyDomeTexName(NOISE_MACRO_DEFAULT_MATERIAL_NAME)
{

}

IAtmosphere::~IAtmosphere()
{
	ReleaseCOM(m_pVB_Gpu);
	ReleaseCOM(m_pIB_Gpu);
}


void		IAtmosphere::SetFogEnabled(bool isEnabled)
{
	mFogEnabled = isEnabled;
	mFogCanUpdateToGpu = true;
}

bool IAtmosphere::IsFogEnabled()
{
	return mFogEnabled;
}

void		IAtmosphere::SetFogParameter(float fogNear, float fogFar, NVECTOR3 color)
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
	mFogColor = color;
	mFogCanUpdateToGpu = true;
}

void		IAtmosphere::SetSkyDomeTexture(N_UID matName)
{
	//validation will be done in NoiseRenderer
	mSkyDomeTexName = matName;
}

void		IAtmosphere::SetSkyBoxTexture(N_UID cubeMapMatName)
{
	mSkyBoxCubeTexName = cubeMapMatName;
}

N_UID IAtmosphere::GetSkyTextureUID()
{
	if (mSkyType == NOISE_ATMOSPHERE_SKYTYPE::NOISE_ATMOSPHERE_SKYTYPE_BOX)
	{
		return mSkyBoxCubeTexName;
	}
	else if (mSkyType == NOISE_ATMOSPHERE_SKYTYPE::NOISE_ATMOSPHERE_SKYTYPE_DOME)
	{
		return mSkyDomeTexName;
	}
	else
	{
		return "";
	}
}

NOISE_ATMOSPHERE_SKYTYPE IAtmosphere::GetSkyType()
{
	return mSkyType;
}

float IAtmosphere::GetSkyboxWidth()
{
	return mSkyBoxWidth;
}

float IAtmosphere::GetSkyboxHeight()
{
	return mSkyBoxHeight;
}

float IAtmosphere::GetSkyboxDepth()
{
	return mSkyBoxDepth;
}


/***************************************************************
									PRIVATE
*****************************************************************/

bool  IAtmosphere::mFunction_UpdateDataToVideoMem(const std::vector<N_SimpleVertex>& targetVB,const std::vector<UINT>& targetIB)
{
	//check if buffers have been created
	ReleaseCOM(m_pVB_Gpu);
	mVB_Mem.clear();
	ReleaseCOM(m_pIB_Gpu);
	mIB_Mem.clear();

	//this function could be externally invoked by ModelLoader..etc
	mVB_Mem =targetVB;
	mIB_Mem = targetIB;


#pragma region CreateGpuBuffers
	//Prepare to update to video memory, fill in SUBRESOURCE description structure
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &mVB_Mem.at(0);
	UINT vertexCount = mVB_Mem.size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &mIB_Mem.at(0);
	UINT indexCount = mIB_Mem.size();

	//------Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_SimpleVertex)* vertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	vbd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	vbd.StructureByteStride = 0;

	//Create Buffers
	HRESULT hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu);
	HR_DEBUG(hr, "IAtmosphere : Failed to create vertex buffer ! ");


	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(int) * indexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;//这个是GPU能对其读写,IMMUTABLE是GPU只读
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0; //CPU啥都干不了  D3D_USAGE
	ibd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE 具体查MSDN
	ibd.StructureByteStride = 0;

	//Create Buffers
	hr = g_pd3dDevice11->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu);
	HR_DEBUG(hr, "IAtmosphere : Failed to create index buffer ! ");

#pragma endregion CreateGpuBuffers

	return true;
}

