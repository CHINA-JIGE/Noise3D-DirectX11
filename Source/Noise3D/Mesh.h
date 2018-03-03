
/***********************************************************************

                           h£ºNoiseMesh

************************************************************************/

#pragma once

namespace Noise3D
{
	//correspond to one draw call of MESH
	struct N_MeshSubsetInfo
	{
		N_MeshSubsetInfo() { ZeroMemory(this, sizeof(*this)); }
		UINT		startPrimitiveID;
		UINT		primitiveCount;
		std::string		matName;
	};


	class /*_declspec(dllexport)*/ IMesh
		: public CRenderSettingBlendMode,
		public CRenderSettingCullMode,
		public CRenderSettingFillMode,
		public CRenderSettingShadeMode
	{
	public:

		void		ResetMaterialToDefault();

		void		SetMaterial(N_UID matName);

		//subset list of a mesh (one subset for one material)
		void		SetSubsetList(const std::vector<N_MeshSubsetInfo>& subsetList);

		void		GetSubsetList(std::vector<N_MeshSubsetInfo>& outRefSubsetList);

		void		SetPosition(float x, float y, float z);

		void		SetPosition(const NVECTOR3& pos);

		NVECTOR3 GetPosition();

		//Radian angle
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

		UINT	GetIndexCount();

		UINT	GetTriangleCount();

		void		GetVertex(UINT iIndex, N_DefaultVertex& outVertex);

		const	std::vector<N_DefaultVertex>*	GetVertexBuffer() const;

		const	std::vector<UINT>*	GetIndexBuffer() const;

		void		GetWorldMatrix(NMATRIX& outWorldMat,NMATRIX& outWorldInvTransposeMat);

		//WARNING!!!! bounding box is computed without applying a world transformation to vertices
		N_Box	ComputeBoundingBox();

	private:

		friend class IRenderModuleForMesh;
		friend class IRenderModuleForPostProcessing;//ref by qwerty
		friend class IModelLoader;
		friend class IModelProcessor;
		friend class ICollisionTestor;
		friend IFactory<IMesh>;

		//¹¹Ôìº¯Êý
		IMesh();

		~IMesh();

		//this function could be externally invoked by ModelLoader..etc
		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateDataToVideoMem(const std::vector<N_DefaultVertex>& targetVB,const std::vector<UINT>& targetIB);
		
		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateDataToVideoMem();

		//invoked by NoiseRenderer
		void		mFunction_UpdateWorldMatrix();

		//this function use the vertex list of vector<N_DefaultVertex>
		void		mFunction_ComputeBoundingBox();

		void		mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer);

	private:

		ID3D11Buffer*						m_pVB_Gpu;
		ID3D11Buffer*						m_pIB_Gpu;

		float										mRotationX_Pitch;
		float										mRotationY_Yaw;
		float										mRotationZ_Roll;
		float										mScaleX;
		float										mScaleY;
		float										mScaleZ;

		NVECTOR3							mPosition;
		N_Box									mBoundingBox;

		NMATRIX										mMatrixWorld;
		NMATRIX										mMatrixWorldInvTranspose;
		std::vector<N_DefaultVertex>	mVB_Mem;//vertex in CPU memory
		std::vector<UINT>						mIB_Mem;//index in CPU memory
		std::vector<N_MeshSubsetInfo>mSubsetInfoList;//store [a,b] of a subset

	};
};