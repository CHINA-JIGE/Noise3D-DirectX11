
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

		GraphicObject*		CreateGraphicObj(N_UID objName);

		GraphicObject*		GetGraphicObj(N_UID objName);

		GraphicObject*		GetGraphicObj(UINT index);

		bool			DestroyGraphicObj(N_UID objName);

		bool			DestroyGraphicObj(GraphicObject* pObj);

		void			DestroyAllGraphicObj();

		UINT		GetGraphicObjCount();

	private:

		friend IFactory<GraphicObjectManager>;

		GraphicObjectManager();

		~GraphicObjectManager();

	};
};