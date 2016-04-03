
/***********************************************************************

                           h£ºNoiseMesh

************************************************************************/

#pragma once


class _declspec(dllexport) NoiseMesh : private NoiseFileManager,public NoiseClassLifeCycle
{
	friend class NoiseScene;
	friend class NoiseRenderer;

public:
	//¹¹Ôìº¯Êý
	NoiseMesh();

	void	CreatePlane(float fWidth,float fDepth,UINT iRowCount =5,UINT iColumnCount =5);

	void CreateBox(float fWidth,float fHeight,float fDepth,UINT iDepthStep=3,UINT iWidthStep =3,UINT iHeightStep= 3);

	void	CreateSphere(float fRadius,UINT iColumnCount=20,UINT iRingCount=20);

	void CreateCylinder(float fRadius,float fHeight,UINT iColumnCount =40,UINT iRingCount =8);

	void	SetMaterial(std::string matName);

	BOOL	LoadFile_STL(NFilePath pFilePath);

	BOOL	LoadFile_OBJ(NFilePath pFilePath);

	BOOL	LoadFile_3DS(NFilePath pFilePath);

	void		SetPosition(float x,float y,float z);

	void		SetRotation(float angleX, float angleY, float angleZ);

	void		SetRotationX_Pitch(float angleX);

	void		SetRotationY_Yaw(float angleY);

	void		SetRotationZ_Roll(float angleZ);

	void		SetScale(float scaleX, float scaleY, float scaleZ);

	void		SetScaleX(float scaleX);

	void		SetScaleY(float scaleY);

	void		SetScaleZ(float scaleZ);

	UINT		GetVertexCount();

	void		GetVertex(UINT iIndex,N_DefaultVertex& outVertex);

	void		GetVertexBuffer(std::vector<N_DefaultVertex>& outBuff);

	NVECTOR3		ComputeBoundingBoxMax();

	NVECTOR3		ComputeBoundingBoxMin();

private:

	void	Destroy();

	//the Parameter "iVertexCount" remind me to update the VertexCount in different Creating Method
	BOOL	mFunction_CreateGpuBuffers(D3D11_SUBRESOURCE_DATA* pVertexDataInMem,int iVertexCount,D3D11_SUBRESOURCE_DATA* pIndexDataInMem,int iIndexCount);

	//invoked by NoiseRenderer
	void		NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateWorldMatrix();
	
	//this function use the vertex list of vector<N_DefaultVertex>
	void		mFunction_ComputeBoundingBox();

	void		mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer);

	NVECTOR2	mFunction_ComputeTexCoord_SphericalWrap(NVECTOR3 vBoxCenter,NVECTOR3 vPoint);

private:
	NoiseScene* m_pFatherScene;

	//internal mesh generator
	NoiseGeometryMeshGenerator mMeshGenerator;

	UINT										mVertexCount;
	UINT										mIndexCount;
	ID3D11Buffer*						m_pVB_Gpu;
	ID3D11Buffer*						m_pIB_Gpu;

	float										mRotationX_Pitch;
	float										mRotationY_Yaw;
	float										mRotationZ_Roll;
	float										mScaleX;
	float										mScaleY;
	float										mScaleZ;

	NVECTOR3*							m_pPosition;
	NVECTOR3*							m_pBoundingBox_Min;
	NVECTOR3*							m_pBoundingBox_Max;
	NMATRIX*										m_pMatrixWorld;
	NMATRIX*										m_pMatrixWorldInvTranspose;
	std::vector<N_DefaultVertex>*			m_pVB_Mem;//vertex in CPU memory
	std::vector<UINT>*							m_pIB_Mem;//index in CPU memory
	std::vector<N_MeshSubsetInfo>*		m_pSubsetInfoList;//store [a,b] of a subset

};
