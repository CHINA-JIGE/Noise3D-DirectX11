
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


	class /*_declspec(dllexport)*/ Mesh
		: public GeometryEntity<N_DefaultVertex, uint32_t>,//derived from ISceneObject blabla
		public CRenderSettingBlendMode,
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

		virtual N_AABB ComputeWorldAABB_Accurate() override;

		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType()const override;

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

	};
};