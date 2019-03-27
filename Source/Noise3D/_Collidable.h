
/***********************************************************************

								h£ºCollidable

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ Collidable
	{
	public:

		Collidable() : mCollidable(true){}

		bool IsCollidable() {return mCollidable; }

		void SetCollidable(bool collidable) { mCollidable = collidable; }

	private:

		bool mCollidable;
	};
}