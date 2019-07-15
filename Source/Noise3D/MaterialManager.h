/***********************************************************************

								h£ºMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ MaterialManager
		:public IFactoryEx<LambertMaterial, GI::PbrtMaterial>
	{
	public:

		LambertMaterial*		CreateLambertMaterial(N_UID matName,const N_LambertMaterialDesc& matDesc);

		LambertMaterial*		GetDefaultLambertMaterial();

		GI::PbrtMaterial* CreateAdvancedMaterial(N_UID matName, const GI::N_PbrtMatDesc& matDesc);

		GI::PbrtMaterial*		GetDefaultAdvancedMaterial();


	private:

		friend  class SceneManager;

		friend IFactory<MaterialManager>;

		MaterialManager();

		~MaterialManager();

		void		mFunction_CreateDefaultMaterial();

	};
}