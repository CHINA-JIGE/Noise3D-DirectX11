
/***********************************************************************

								h£ºMeshManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ LogicalShapeManager :
		public IFactoryEx<LogicalBox, LogicalSphere, LogicalRect>
	{
	public:

		LogicalBox*	CreateBox(SceneNode* pAttachedNode, N_UID uid, Vec3 size);

		LogicalSphere*CreateSphere(SceneNode* pAttachedNode, N_UID uid, float radius);

		LogicalRect* CreateRect(SceneNode* pAttachedNode, N_UID uid,Vec2 size, NOISE_RECT_ORIENTATION ori = NOISE_RECT_ORIENTATION::RECT_XZ);

	private:

		friend IFactory<LogicalShapeManager>;

		LogicalShapeManager();

		~LogicalShapeManager();

	};
}