
/***********************************************************************

                           h：NoiseMesh

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseMesh : private NoiseFileManager
{
	friend class NoiseScene;
	friend class NoiseRenderer;

public:
	//构造函数
	NoiseMesh();

	~NoiseMesh();

	void	CreatePlane(float fWidth,float fHeight,UINT iRowCount =5,UINT iColumnCount =5);

	void CreateBox(float fWidth,float fHeight,float fDepth,UINT iDepthStep=3,UINT iWidthStep =3,UINT iHeightStep= 3);

	void	CreateSphere(float fRadius,UINT iColumnCount=20,UINT iRingCount=20);

	void CreateCylinder(float fRadius,float fHeight,UINT iColumnCount =40,UINT iRingCount =8);

	void	SetMaterial(N_Material Mat);

	BOOL LoadFile_STL(char* pFilePath);

	BOOL  AddToRenderList();

	void SetPosition(float x,float y,float z);

	void SetRotation(float angleX, float angleY, float angleZ);

	void SetRotationX_Pitch(float angleX);

	void SetRotationY_Yaw(float angleY);

	void SetRotationZ_Roll(float angleZ);

	void	SetScale(float scaleX, float scaleY, float scaleZ);

	void	SetScaleX(float scaleX);

	void	SetScaleY(float scaleY);

	void	SetScaleZ(float scaleZ);

	int	GetVertexCount();

	void GetVertex(UINT iIndex,N_DefaultVertex& outVertex);

	void GetVertexBuffer(std::vector<N_DefaultVertex>& outBuff);

private:

	//the Parameter "iVertexCount" remind me to update the VertexCount in different Creating Method
	BOOL	mFunction_CreateGpuBuffers(D3D11_SUBRESOURCE_DATA* pVertexDataInMem,int iVertexCount,D3D11_SUBRESOURCE_DATA* pIndexDataInMem,int iIndexCount);

	void		mFunction_Build_A_Quad(NVECTOR3 vOriginPoint,NVECTOR3 vBasisVector1,NVECTOR3 vBasisVector2,UINT StepCount1,UINT StepCount2,UINT iBaseIndex);

	void		mFunction_UpdateWorldMatrix();
	
	//this function use the vertex list of vector<N_DefaultVertex>
	void		mFunction_ComputeBoundingBox();

	void		mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer);

	NVECTOR2	mFunction_ComputeTexCoord_SphericalWrap(NVECTOR3 vBoxCenter,NVECTOR3 vPoint);

private:
	NoiseScene* m_pFatherScene;

	UINT										m_VertexCount;
	UINT										m_IndexCount;
	UINT										m_TriangleCount;
	ID3D11Buffer*						m_pVertexBuffer;
	ID3D11Buffer*						m_pIndexBuffer;

	float										m_RotationX_Pitch;
	float										m_RotationY_Yaw;
	float										m_RotationZ_Roll;
	float										m_ScaleX;
	float										m_ScaleY;
	float										m_ScaleZ;

	NVECTOR3*							m_pPosition;
	NVECTOR3*							m_pBoundingBox_Min;
	NVECTOR3*							m_pBoundingBox_Max;
	NMATRIX*										m_pMatrixWorld;
	NMATRIX*										m_pMatrixWorldInvTranspose;
	std::vector<N_DefaultVertex>*			m_pVertexInMem;//储存在内存的vertex
	std::vector<UINT>*							m_pIndexInMem;//储存在内存的index
	std::vector<N_Material>*					m_pMaterialInMem;


};
