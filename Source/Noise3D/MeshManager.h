
/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IMeshManager:
		public IFactory<IMesh>
	{
	public:

		IMesh*					CreateMesh(N_UID meshName);

		IMesh*					GetMesh(N_UID meshName);

		IMesh*					GetMesh(UINT index);

		BOOL					DestroyMesh(N_UID meshName);

		BOOL					DestroyMesh(IMesh* ppMesh);

		void						DestroyAllMesh();

		UINT					GetMeshCount();


	private:

		friend IFactory<IMeshManager>;

		IMeshManager();

		~IMeshManager();

	};
};