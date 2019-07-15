
/***********************************************************************

								h£ºCollidable

************************************************************************/

#pragma once

namespace Noise3D
{
	//(2019.3.29)can be inherited by ICollidableSceneObject
	class /*_declspec(dllexport)*/ Collidable
	{
	public:

		Collidable() : mCollidable(true){}

		bool IsCollidable() {return mCollidable; }//some class remains constant not collidable

		void SetCollidable(bool collidable) { mCollidable = collidable; }

	private:

		bool mCollidable;
	};
}