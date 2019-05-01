
/******************************************************

								  h£ºMesh

******************************************************/

#pragma once

namespace Noise3D
{
	//correspond to one draw call of MESH
	struct N_MeshSubsetInfo
	{
		N_MeshSubsetInfo():
			startPrimitiveID(0),
			primitiveCount(0),
			matName("")
		{ }
		UINT		startPrimitiveID;
		UINT		primitiveCount;
		std::string		matName;
	};


	struct N_MeshPbrtSubsetInfo
	{
		N_MeshPbrtSubsetInfo() :
			startPrimitiveID(0),
			primitiveCount(0),
			pMat(nullptr)
		{ }
		UINT		startPrimitiveID;
		UINT		primitiveCount;
		GI::PbrtMaterial* pMat;
	};

	class /*_declspec(dllexport)*/ Mesh
		: public GeometryEntity<N_DefaultVertex, uint32_t>,//derived from ISceneObject blabla
		public CRenderSettingBlendMode,
		public CRenderSettingCullMode,
		public CRenderSettingFillMode,
		public CRenderSettingShadeMode
	{
	public:

		void		ResetMaterialToDefault();

		//default material applied to the whole mesh
		void		SetMaterial(N_UID matName);

		//subset list of a mesh (one subset for one material)
		void		SetSubsetList(const std::vector<N_MeshSubsetInfo>& subsetList);

		void		GetSubsetList(std::vector<N_MeshSubsetInfo>& outRefSubsetList);

		//PBRT material
		void		SetPbrtMaterialSubset(const std::vector<N_MeshPbrtSubsetInfo>& subsetList);

		GI::PbrtMaterial*	GetPbrtMaterial(int triangleId);//unlike rasterization, getting the whole subset list is unnecessary

		virtual N_AABB ComputeWorldAABB_Accurate() override;

		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

		virtual N_BoundingSphere ComputeWorldBoundingSphere_Accurate() override;

		//is bvh tree manually built (but up-to-date bvh tree is not guaranteed)
		bool IsBvhTreeBuilt();

		//build bvh tree for triangles
		void RebuildBvhTree();

		BvhTreeForTriangularMesh& GetBvhTree();

	private:

		friend class IRenderModuleForMesh;
		friend class IRenderModuleForPostProcessing;//ref by qwerty3d
		friend class MeshLoader;
		friend class ModelProcessor;
		friend class CollisionTestor;
		friend class MeshManager;
		friend IFactory<Mesh>;

		Mesh();

		~Mesh();

	private:

		std::vector<N_MeshSubsetInfo>mSubsetInfoList;//store [a,b] of a subset

		std::vector<N_MeshPbrtSubsetInfo> mPbrtMatSubsetInfoList; //store[a, b] of a PBRT material subset

		BvhTreeForTriangularMesh mBvhTreeLocalSpace;//BVH tree of triangles

		bool mIsBvhTreeBuilt;

	};
};