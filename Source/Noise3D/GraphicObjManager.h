
/***********************************************************************

				h£ºGraphic Object Manager

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ IGraphicObjectManager :
		public IFactory<IGraphicObject>
	{
	public:

		IGraphicObject*		CreateGraphicObj(N_UID objName);

		IGraphicObject*		GetGraphicObj(N_UID objName);

		IGraphicObject*		GetGraphicObj(UINT index);

		bool			DestroyGraphicObj(N_UID objName);

		bool			DestroyGraphicObj(IGraphicObject* pObj);

		void			DestroyAllGraphicObj();

		UINT		GetGraphicObjCount();

	private:

		friend IFactory<IGraphicObjectManager>;

		IGraphicObjectManager();

		~IGraphicObjectManager();

	};
};