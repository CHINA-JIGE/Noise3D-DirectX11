
/***********************************************************************

				h£ºModel Processor (all kinds of operation on
					loaded model , like vertices-welding , mesh-smoothing,
					mesh-simplification, mesh-LOD-generation)

************************************************************************/

#pragma once

namespace Noise3D
{
	typedef std::vector<std::vector<UINT>> N_AdjacentList;


	class /*_declspec(dllexport)*/ IModelProcessor
	{
	public:

		void WeldVertices(IMesh* pTargetMesh);//vertices will same position will be weld as one

		void WeldVertices(IMesh* pTargetMesh,float PositionEqualThreshold);//vertices will same position will be weld as one

		void MeshSimplify(IMesh* pTargetMesh, float PositionEqualThreshold, float visualImportanceWeightThreshold);

		//vertices need to welded before smoothing.
		//but this algorithm seems to be not robust enough...
		void Smooth_Laplacian(IMesh* pTargetMesh);

	private:

		friend class IFactory<IModelProcessor>;

		IModelProcessor();

		~IModelProcessor();

		static float static_PositionEqualThreshold;

	};

};