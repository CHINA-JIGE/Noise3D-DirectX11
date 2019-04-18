
/***********************************************************

								GI Common

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class /*_declspec(dllexport)*/ IGiRenderable:
			public ICollidableSceneObject,
			public GI::IAdvancedGiMaterialOwner
		{

		};
	}
}