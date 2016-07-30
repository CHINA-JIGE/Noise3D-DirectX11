
/***********************************************************************

                           h£ºNoiseMesh

************************************************************************/

#pragma once

namespace Noise3D
{
	class _declspec(dllexport) IMesh : private IFileManager
	{
		friend  class IScene;
		friend  class IRenderer;

	public:
		//¹¹Ôìº¯Êý
		IMesh();

		void	CreatePlane(float fWidth, float fDepth, UINT iRowCount = 5, UINT iColumnCount = 5);

		void CreateBox(float fWidth, float fHeight, float fDepth, UINT iDepthStep = 3, UINT iWidthStep = 3, UINT iHeightStep = 3);

		void	CreateSphere(float fRadius, UINT iColumnCount = 20, UINT iRingCount = 20);

		void CreateCylinder(float fRadius, float fHeight, UINT iColumnCount = 40, UINT iRingCount = 8);

		void	SetMaterial(std::string matName);

		BOOL	LoadFile_STL(NFilePath pFilePath);

		BOOL	LoadFile_OBJ(NFilePath pFilePath);

		BOOL	LoadFile_3DS(NFilePath pFilePath);

		void		SetPosition(float x, float y, float z);

		void		SetPosition(const NVECTOR3& pos);

		void		SetRotation(float angleX, float angleY, float angleZ);

		void		SetRotationX_Pitch(float angleX);

		void		SetRotationY_Yaw(float angleY);

		void		SetRotationZ_Roll(float angleZ);

		void		SetScale(float scaleX, float scaleY, float scaleZ);

		void		SetScaleX(float scaleX);

		void		SetScaleY(float scaleY);

		void		SetScaleZ(float scaleZ);

		UINT		GetVertexCount();

		void		GetVertex(UINT iIndex, N_DefaultVertex& outVertex);

		void		GetVertexBuffer(std::vector<N_DefaultVertex>& outBuff);

		void		GetWorldMatrix(NMATRIX& outWorldMat,NMATRIX& outWorldInvTMat);

		N_Box		ComputeBoundingBox();

	private:

		void	Destroy();

		//the Parameter "iVertexCount" remind me to update the VertexCount in different Creating Method
		BOOL	mFunction_CreateGpuBuffers(D3D11_SUBRESOURCE_DATA* pVertexDataInMem, int iVertexCount, D3D11_SUBRESOURCE_DATA* pIndexDataInMem, int iIndexCount);

		//invoked by NoiseRenderer
		void		mFunction_UpdateWorldMatrix();

		//this function use the vertex list of vector<N_DefaultVertex>
		void		mFunction_ComputeBoundingBox();

		void		mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer);

		NVECTOR2	mFunction_ComputeTexCoord_SphericalWrap(NVECTOR3 vBoxCenter, NVECTOR3 vPoint);

	private:
		IScene* m_pFatherScene;

		//internal mesh generator
		IGeometryMeshGenerator mMeshGenerator;

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
		N_Box									mBoundingBox;

		NMATRIX*										m_pMatrixWorld;
		NMATRIX*										m_pMatrixWorldInvTranspose;
		std::vector<N_DefaultVertex>*			m_pVB_Mem;//vertex in CPU memory
		std::vector<UINT>*							m_pIB_Mem;//index in CPU memory
		std::vector<N_MeshSubsetInfo>*		m_pSubsetInfoList;//store [a,b] of a subset

	};
}