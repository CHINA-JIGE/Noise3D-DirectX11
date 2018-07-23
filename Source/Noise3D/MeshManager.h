
/***********************************************************************

								h£ºMeshManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IMeshManager:
		public IFactory<IMesh>
	{
	public:

		IMesh*				CreateMesh(N_UID meshName);

		IMesh*				GetMesh(N_UID meshName);

		IMesh*				GetMesh(UINT index);

		bool					DestroyMesh(N_UID meshName);

		bool					DestroyMesh(IMesh* pMesh);

		void					DestroyAllMesh();

		UINT				GetMeshCount();

		bool					IsMeshExisted(N_UID meshName);

	private:

		friend IFactory<IMeshManager>;

		IMeshManager();

		~IMeshManager();

	};
};