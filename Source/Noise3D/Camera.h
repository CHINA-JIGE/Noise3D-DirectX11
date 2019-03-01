
/***********************************************************************

                           h£ºCamera

************************************************************************/

#pragma once

#include "CameraTransform.h"

namespace Noise3D
{
	class /*_declspec(dllexport)*/ Camera:
		public CameraTransform,
		public ISceneObject
	{
	public:

		void		Destroy();

		void		fps_MoveForward(float fSignedDistance, bool enableYAxisMovement = false);

		void		fps_MoveRight(float fSignedDistance, bool enableYAxisMovement = false);

		void		fps_MoveUp(float fSignedDistance);

		void		OptimizeForQwertyPass1(const Mesh* pScreenDescriptor);

		virtual	N_AABB GetLocalAABB() override;

		virtual	N_AABB ComputeWorldAABB_Accurate() override;

		virtual	NOISE_SCENE_OBJECT_TYPE GetObjectType() override;

	private:

		friend  class Renderer;

		friend IFactory<Camera>;

		Camera();

		~Camera();

	};

};


