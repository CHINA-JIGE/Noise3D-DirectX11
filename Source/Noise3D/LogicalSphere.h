
/*********************************************************

								Logical Sphere
		geometry representation that is not based on polygon.
		instead, it's based on analytic description

************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ LogicalSphere :
		public ILogicalShape
	{
	public:

		bool ComputeNormal(NVECTOR3 pos);

	private:

	};

}