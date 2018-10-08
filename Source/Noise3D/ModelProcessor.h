
/***********************************************************************

				h£ºModel Processor (all kinds of operation on
					loaded model , like vertices-welding , mesh-smoothing,
					mesh-simplification, mesh-LOD-generation)

************************************************************************/

#pragma once

namespace Noise3D
{
	class Mesh;
	typedef std::vector<std::vector<UINT>> N_AdjacentList;


	class /*_declspec(dllexport)*/ ModelProcessor
	{
	public:

		void WeldVertices(Mesh* pTargetMesh);//vertices will same position will be weld as one

		void WeldVertices(Mesh* pTargetMesh,float PositionEqualThreshold);//vertices will same position will be weld as one

		void MeshSimplify(Mesh* pTargetMesh, float PositionEqualThreshold, float visualImportanceWeightThreshold);

		//vertices need to welded before smoothing.
		//but this algorithm seems to be not robust enough...
		void Smooth_Laplacian(Mesh* pTargetMesh);

	private:

		friend class IFactory<ModelProcessor>;

		ModelProcessor();

		~ModelProcessor();

		static float static_PositionEqualThreshold;

	};

};