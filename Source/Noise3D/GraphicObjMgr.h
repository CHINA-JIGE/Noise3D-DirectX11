#pragma once
/***********************************************************************

h£ºNoiseMaterialManager

************************************************************************/

#pragma once

namespace Noise3D
{
	enum NOISE_GRAPHIC_OBJECT_TYPE
	{
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D = 0,
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D = 1,
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D = 2,
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D = 3,
		NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D = 4,
		NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D = 5,
	};

	class /*_declspec(dllexport)*/ IGraphicObjectManager :
		public IFactory<IGraphicObject>
	{
	public:

		IGraphicObject*		CreateGraphicObj(N_UID objName);

		IGraphicObject*		GetGraphicObj(N_UID objName);

		IGraphicObject*		GetGraphicObj(UINT index);

		bool							DestroyGraphicObj(N_UID objName);

		bool							DestroyGraphicObj(IGraphicObject* pObj);

		void							DestroyAllGraphicObj();

		UINT						GetGraphicObjCount();

	private:

		friend IFactory<IGraphicObjectManager>;

		IGraphicObjectManager();

		~IGraphicObjectManager();

	};
};