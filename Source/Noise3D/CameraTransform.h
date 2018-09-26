
/***********************************************************************

                           h£ºCamera

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ CameraTransform: 
		public RigidTransform
	{
	public:

		//Lookat & Position
		void		LookAt(NVECTOR3 vLookat);

		void		LookAt(float x, float y, float z);
		
		NVECTOR3	GetLookAtPos();

		NVECTOR3	GetDirection();

	private:

		NVECTOR3	mLookat;

		NVECTOR3	mDirection;
	};
}