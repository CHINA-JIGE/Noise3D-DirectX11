
/***********************************************************

								GI Common

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		//(2019.4.17) inherited by ISceneObject
		class /*_declspec(dllexport)*/ IGiLightSource
		{
		public:

			//require GI light source to override this explicitly
			virtual bool IsGiLightSource() { return false; }
		};

		class /*_declspec(dllexport)*/ IGiRenderable:
			public ICollidableSceneObject,
			public GI::IAdvancedGiMaterialOwner,
			public GI::IGiLightSource//for GI render system
		{

		};
	}
}