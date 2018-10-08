/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IMaterialManager
		:public IFactory<Material>
	{
	public:


		Material*		CreateMaterial(N_UID matName,const N_MaterialDesc& matDesc);

		UINT			GetMaterialCount();

		Material*		GetDefaultMaterial();

		Material*		GetMaterial(N_UID matName);

		bool		DeleteMaterial(N_UID matName);

		void		DeleteAllMaterial();

		bool		ValidateUID(N_UID matName);

	private:

		friend  class IScene;

		friend IFactory<IMaterialManager>;

		IMaterialManager();

		~IMaterialManager();

		void		mFunction_CreateDefaultMaterial();

	};
}