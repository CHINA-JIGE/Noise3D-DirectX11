
/***********************************************************************

								 h£ºScene Node
				desc: a single node in a scene graph, storing 
				transformation and some geometry, object binding infos
				(similar to OGRE's idea)

************************************************************************/

#pragma once

namespace Noise3D
{
	class SceneNode:
		public AffineTransform
	{
	public:

	private:
	
		std::vector<ISceneObject*> mSceneObjectList;
	};
}