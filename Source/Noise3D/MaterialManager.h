/***********************************************************************

								h£ºMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ MaterialManager
		:public IFactory<LambertMaterial>
	{
	public:

		LambertMaterial*		CreateMaterial(N_UID matName,const N_LambertMaterialDesc& matDesc);

		LambertMaterial*		GetDefaultMaterial();

	private:

		friend  class SceneManager;

		friend IFactory<MaterialManager>;

		MaterialManager();

		~MaterialManager();

		void		mFunction_CreateDefaultMaterial();

	};
}