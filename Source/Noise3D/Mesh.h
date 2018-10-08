
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


	class /*_declspec(dllexport)*/ Mesh
		: public CRenderSettingBlendMode,
		public CRenderSettingCullMode,
		public CRenderSettingFillMode,
		public CRenderSettingShadeMode,
		public AffineTransform
	{
	public:

		void		ResetMaterialToDefault();

		void		SetMaterial(N_UID matName);

		//subset list of a mesh (one subset for one material)
		void		SetSubsetList(const std::vector<N_MeshSubsetInfo>& subsetList);

		void		GetSubsetList(std::vector<N_MeshSubsetInfo>& outRefSubsetList);

		UINT	GetIndexCount();

		UINT	GetTriangleCount();

		void		GetVertex(UINT iIndex, N_DefaultVertex& outVertex);

		const	std::vector<N_DefaultVertex>*	GetVertexBuffer() const;

		const	std::vector<UINT>*	GetIndexBuffer() const;

		//WARNING!!!! bounding box is computed without applying a world transformation to vertices
		N_Box	ComputeBoundingBox();

	private:

		friend class IRenderModuleForMesh;
		friend class IRenderModuleForPostProcessing;//ref by qwerty3d
		friend class ModelLoader;
		friend class ModelProcessor;
		friend class CollisionTestor;
		friend IFactory<Mesh>;

		Mesh();

		~Mesh();

		//this function could be externally invoked by ModelLoader..etc
		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateDataToVideoMem(const std::vector<N_DefaultVertex>& targetVB,const std::vector<UINT>& targetIB);
		
		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateDataToVideoMem();

		//this function use the vertex list of vector<N_DefaultVertex>
		void		mFunction_ComputeBoundingBox();

		void		mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer);

	private:

		ID3D11Buffer*						m_pVB_Gpu;
		ID3D11Buffer*						m_pIB_Gpu;
		N_Box									mBoundingBox;
		std::vector<N_DefaultVertex>	mVB_Mem;//vertex in CPU memory
		std::vector<UINT>						mIB_Mem;//index in CPU memory
		std::vector<N_MeshSubsetInfo>mSubsetInfoList;//store [a,b] of a subset

	};
};