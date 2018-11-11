
/***********************************************************************

								h£ºMeshManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ MeshManager:
		public IFactory<Mesh>
	{
	public:

		Mesh*				CreateMesh(N_UID meshName);

		Mesh*				GetMesh(N_UID meshName);

		Mesh*				GetMesh(UINT index);

		bool					DestroyMesh(N_UID meshName);

		bool					DestroyMesh(Mesh* pMesh);

		void					DestroyAllMesh();

		UINT				GetMeshCount();

		bool					IsMeshExisted(N_UID meshName);

	private:

		friend IFactory<MeshManager>;

		MeshManager();

		~MeshManager();

	};
};