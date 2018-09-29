
/***********************************************************************

                           h£ºCamera

************************************************************************/

#pragma once

#include "CameraTransform.h"

namespace Noise3D
{
	class /*_declspec(dllexport)*/ ICamera:
		public CameraTransform
	{
	public:

		void				Destroy();

		void				fps_MoveForward(float fSignedDistance, bool enableYAxisMovement = false);

		void				fps_MoveRight(float fSignedDistance, bool enableYAxisMovement = false);

		void				fps_MoveUp(float fSignedDistance);

		void				OptimizeForQwertyPass1(const IMesh* pScreenDescriptor);

	private:

		friend  class IRenderer;

		friend IFactory<ICamera>;

		ICamera();

		~ICamera();

	};

};


