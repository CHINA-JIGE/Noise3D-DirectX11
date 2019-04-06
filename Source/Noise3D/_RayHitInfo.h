
/*********************************************************

								h£ºRay hit info

**********************************************************/

#pragma once

namespace Noise3D
{

	//record of a single successful collision between Ray and XX(simple hitinfo)
	struct N_RayHitInfo
	{
		//N_RayHitInfo():t(-std::numeric_limits<float>::infinity()) {}
		N_RayHitInfo(float _t, Vec3 _pos, Vec3 _normal) :
			t(_t), pos(_pos), normal(_normal) {}

		//validate the hit, check param t and see whether it's infinity(it shouldn't be)
		bool IsValid() { return (t != std::numeric_limits<float>::infinity()); }

		float t;//ray's hit parameter t
		Vec3 pos;//hit point's pos
		Vec3 normal;//hit point's normal vector
	};

	//extension of N_RayHitInfo, material and the hitting ray info are carried
	struct N_RayHitInfoForPathTracer :
		N_RayHitInfo
	{
		N_RayHitInfoForPathTracer(GI::AdvancedGiMaterial* _pMat,
			float _t, Vec3 _pos, Vec3 _normal): N_RayHitInfo(_t, _pos, _normal),pMat(_pMat){}

		N_RayHitInfoForPathTracer(GI::AdvancedGiMaterial* _pMat, 
			const N_RayHitInfo& e) :	N_RayHitInfo(e), pMat(_pMat) {}

		GI::AdvancedGiMaterial* pMat;//material info can be retrieved.
		//N_Ray ray;//the ray
	};

	//base class
	template<typename hitInfo_t>
	struct N_RayHitResultTemplate
	{
		N_RayHitResultTemplate() {};
		N_RayHitResultTemplate(const hitInfo_t& rhs) { hitList = rhs.hitList; }

		//sort by depth (in ascending order)(or ray's parameter t)
		void SortByDepth()
		{
			// "<" in predicate will cause ASCENDING order
			std::sort(hitList.begin(), hitList.end(),
				[](const hitInfo_t &v1, const hitInfo_t &v2)->bool {return v1.t < v2.t; });
		}

		bool HasAnyHit()
		{
			return !hitList.empty();
		}

		void Union(const N_RayHitResultTemplate<hitInfo_t>& rhs)
		{
			if (rhs.hitList.empty())return;
			hitList.insert(hitList.end(), rhs.hitList.begin(), rhs.hitList.end());
		}

		std::vector<hitInfo_t> hitList;
	};

	//collection of RayHitInfo
	struct N_RayHitResult:
		public N_RayHitResultTemplate<N_RayHitInfo>
	{};

	//collection of RayHitInfoForPathTracer
	struct N_RayHitResultForPathTracer :
		N_RayHitResultTemplate<N_RayHitInfoForPathTracer>
	{
		//get the closest hit's index in hit list
		int GetClosestHitIndex()
		{
			uint32_t index = 0xffffffff;
			//float closest_dist = std::numeric_limits<float>::infinity();
			float closest_t = std::numeric_limits<float>::infinity();
			for (uint32_t i = 0; i < hitList.size(); ++i)
			{
				N_RayHitInfoForPathTracer& info = hitList.at(i);
				//float dist = info.ray.Distance(info.t);
				float t = info.t;
				if (t < closest_t)
				{
					closest_t = t;
					index = i;
				}
			}
			return index;
		}
	};

}