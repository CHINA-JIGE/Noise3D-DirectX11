
/***********************************************************************

								h£ºMeshManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ LogicalShapeManager :
		public IFactory<LogicalSphere>,
		public IFactory<LogicalBox>
	{
	public:

	};
}