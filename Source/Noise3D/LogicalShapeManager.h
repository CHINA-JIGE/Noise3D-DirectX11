
/***********************************************************************

								h£ºMeshManager

************************************************************************/

#pragma once

namespace Noise3D
{
	class LogicalBox;
	class LogicalSphere;

	class /*_declspec(dllexport)*/ LogicalShapeManager :
		public IFactoryEx<LogicalBox, LogicalSphere>
	{
	public:

		LogicalBox*	CreateBox(SceneNode* pAttachedNode, N_UID uid);

		LogicalSphere*CreateSphere(SceneNode* pAttachedNode, N_UID uid);

	private:

		friend IFactory<LogicalShapeManager>;

		LogicalShapeManager();

		~LogicalShapeManager();

	};
}