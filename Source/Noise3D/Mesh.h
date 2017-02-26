
/***********************************************************************

                           h£ºNoiseMesh

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IMesh
	{
		friend class IRenderer;
		friend class IModelLoader;
		friend class IModelProcessor;
		friend class ICollisionTestor;

	public:

		void		SetMaterial(N_UID matName);

		void		SetPosition(float x, float y, float z);

		void		SetPosition(const NVECTOR3& pos);

		NVECTOR3 GetPosition();

		void		SetRotation(float angleX, float angleY, float angleZ);

		void		SetRotationX_Pitch(float angleX);

		void		SetRotationY_Yaw(float angleY);

		void		SetRotationZ_Roll(float angleZ);

		void		RotateX_Pitch(float angleX);//relatively

		void		RotateY_Yaw(float angleY);//relatively

		void		RotateZ_Roll(float angleZ);//relatively

		float		GetRotationX_Pitch();

		float		GetRotationY_Yaw();

		float		GetRotationZ_Roll();

		void		SetScale(float scaleX, float scaleY, float scaleZ);

		void		SetScaleX(float scaleX);

		void		SetScaleY(float scaleY);

		void		SetScaleZ(float scaleZ);

		UINT	GetVertexCount();

		void		GetVertex(UINT iIndex, N_DefaultVertex& outVertex);

		const std::vector<N_DefaultVertex>*		GetVertexBuffer();

		const std::vector<UINT>*	GetIndexBuffer();

		void			GetWorldMatrix(NMATRIX& outWorldMat,NMATRIX& outWorldInvTMat);

		//WARNING!!!! bounding box is computed without applying a world transformation to vertices
		N_Box		ComputeBoundingBox();

	private:

		friend IFactory<IMesh>;

		//¹¹Ôìº¯Êý
		IMesh();

		~IMesh();

		//this function could be externally invoked by ModelLoader..etc
		BOOL NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateDataToVideoMem(const std::vector<N_DefaultVertex>& targetVB, const std::vector<UINT>& targetIB);
		
		//invoked by NoiseRenderer
		void		mFunction_UpdateWorldMatrix();

		//this function use the vertex list of vector<N_DefaultVertex>
		void		mFunction_ComputeBoundingBox();

		void		mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer);

	private:



		UINT									mVertexCount;
		UINT									mIndexCount;
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
		std::vector<N_DefaultVertex>*		m_pVB_Mem;//vertex in CPU memory
		std::vector<UINT>*							m_pIB_Mem;//index in CPU memory
		std::vector<N_MeshSubsetInfo>*	m_pSubsetInfoList;//store [a,b] of a subset

	};
};