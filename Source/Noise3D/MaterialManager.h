/***********************************************************************

								h£ºNoiseMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ MaterialManager
		:public IFactory<Material>
	{
	public:


		Material*		CreateMaterial(N_UID matName,const N_MaterialDesc& matDesc);

		Material*		GetDefaultMaterial();

	private:

		friend  class SceneManager;

		friend IFactory<MaterialManager>;

		MaterialManager();

		~MaterialManager();

		void		mFunction_CreateDefaultMaterial();

	};
}