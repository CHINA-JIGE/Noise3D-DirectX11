
/***********************************************************************

				h£ºGraphic Object Manager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ GraphicObjectManager :
		public IFactory<GraphicObject>
	{
	public:

		GraphicObject*	CreateGraphicObject(N_UID objName);

	private:

		friend IFactory<GraphicObjectManager>;

		GraphicObjectManager();

		~GraphicObjectManager();

	};
};