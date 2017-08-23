
/***********************************************************************

				h£ºHash table (spatial hashing) which can be
				used in vertex-clustering scenarios, like vertex-welding ,
				mesh simplification based on vertex clustering

************************************************************************/

#pragma once

namespace Noise3D
{
	//hash table designed for vertex clustering (e.g. in mesh simplification)
	//so every unitBox will only has one 3D point that represent the AVERAGE vertex
	//of this vertex cluster.
	class CSpatialHashTableForVertexClustering
	{
	public:

		CSpatialHashTableForVertexClustering( float unitBoxWidth, float unitBoxHeight,float unitBoxDepth, int bucketCount=256);

		//1.vector p will be added to corresponding unit box
		//2.input one triangle at a time to prevent de-generating circumstances at the beginning
		//3.new vertex&index lists of a mesh  will be generated
		//void AddPoint(NVECTOR3 p,UINT vertexIndex);
		void AddPoint(N_DefaultVertex v1, N_DefaultVertex v2, N_DefaultVertex v3);

		bool IsUnitBoxEmpty(NVECTOR3 p);

		bool IsUnitBoxEmpty(int idX, int idY,int idZ);//integer index of unit box

		NVECTOR3 GetAveragePosition(int unitBoxIdX, int unitBoxIdY, int unitBoxIdZ);

		//get all average position vector 
		void GetSimpifiedMesh(std::vector<N_DefaultVertex>& outVertexList,std::vector<UINT>& outIndexList);

	private:

		int mBucketCount;
		float mUnitBoxWidth;
		float mUnitBoxHeight;
		float mUnitBoxDepth;
		

		struct N_UnitBox
		{
			UINT boxIndexX;
			UINT boxIndexY;
			UINT boxIndexZ;
			N_DefaultVertex avgVertex;
			UINT vertexIndex;
		};
		typedef std::vector<N_UnitBox> N_Bucket;

		std::vector<N_Bucket>*	m_pBucketList;//all  unitBoxes in the Buckets will overlap the whole space

	};

};