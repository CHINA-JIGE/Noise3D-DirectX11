
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

		Mesh*		CreateMesh(SceneNode* pAttachedNode, N_UID meshName);

	private:

		friend IFactory<MeshManager>;

		MeshManager();

		~MeshManager();

	};
};