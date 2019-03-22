
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

		//NVECTOR3 ComputeVec(float theta_pitch, float phi_yaw);

		void SetRadius(float r);

		float GetRadius();

	private:

		LogicalSphere();

		~LogicalSphere();

		float mRadius;

	};

}