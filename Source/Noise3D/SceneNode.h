
/***********************************************************************

								 h£ºScene Node
				desc: a single node in a scene graph, storing 
				transformation and some geometry, object binding infos
				(similar to OGRE's idea)

************************************************************************/

#pragma once

namespace Noise3D
{
	class SceneNode
	{
	public:

		void AttachSceneObject(ISceneObject* pObj);

		AffineTransform& GetTransform();
		
		
		//CreateChildSceneNode()
	
	private:
	
		std::vector<ISceneObject*> mSceneObjectList;

		AffineTransform mTransform;

	};
};