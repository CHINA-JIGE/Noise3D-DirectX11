
/***********************************************************************

				h£ºModel Processor (all kinds of operation on
					loaded model , like vertices-welding , mesh-smoothing,
					mesh-simplification, mesh-LOD-generation)

************************************************************************/

#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ IModelProcessor
	{
	public:

		void WeldVertices(IMesh* pTargetMesh);//vertices will same position will be weld as one


	private:

		friend class IFactory<IModelProcessor>;

		IModelProcessor();

		~IModelProcessor();


	};

};