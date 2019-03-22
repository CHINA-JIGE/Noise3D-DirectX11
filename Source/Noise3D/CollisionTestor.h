
/***********************************************************************

								h£ºCollision Testor
				provide all kinds of collision detection
				between all kinds of objects

************************************************************************/

#pragma once

namespace Noise3D
{
	class IShaderVariableManager;//for gpu-accelerated stuffs

	//record of a single successful collision between Ray and XX
	struct N_RayHitInfo
	{
		N_RayHitInfo():t(-std::numeric_limits<float>::infinity()) {}

		//validate the hit, check param t and see whether it's infinity(it shouldn't be)
		bool IsValid()	{	return (t != std::numeric_limits<float>::infinity());	}

		float t;//ray's hit parameter t
	};

	//...
	struct N_RayHitResult
	{
		N_RayHitResult() {};

		//sort by depth (in ascending order)(or ray's parameter t)
		void SortByDepth()
		{
			// "<" in predicate will cause ASCENDING order
			std::sort(hitList.begin(), hitList.end(),
				[](const N_RayHitInfo &v1, const N_RayHitInfo &v2)->bool {return v1.t < v2.t; });
		}

		bool HasAnyHit()
		{
			return !hitList.empty();
		}

		std::vector<N_RayHitInfo> hitList;
	};

	class /*_declspec(dllexport)*/ CollisionTestor
	{
	public:

		//normalized Coord frame(Cartesian) centered at the middle of screen ,with X & Y both valued in [-1,1]
		void Picking_GpuBased(Mesh* pMesh, const NVECTOR2& mouseNormalizedCoord,std::vector<NVECTOR3>& outCollidedPointList);

		//normalized Coord frame(Cartesian) centered at the middle of screen ,with X & Y both valued in [-1,1]
		UINT Picking_GpuBased(Mesh* pMesh, const NVECTOR2& mouseNormalizedCoord);

		//ray-Aabb intersection. 'slabs' method, can refer to pbrt-v3 or peter-shirley's <Ray Tracing:The Next Week>
		bool IntersectRayAabb(const N_Ray& ray, const N_AABB& aabb, N_RayHitResult& outHitRes);

		//ray-sphere intersecton. simply solve an quadratic equation
		bool IntersectRaySphere(const N_Ray& ray, const LogicalSphere& s, N_RayHitResult& outHitRes);

		//ray-triangle intersection
		bool IntersectRayTriangle(const N_Ray& ray, NVECTOR3 v0, NVECTOR3 v1, NVECTOR3 v2, N_RayHitInfo& outHitInfo);

		//ray-Mesh intersection. cpu impl.
		bool IntersectRayMesh(const N_Ray& ray, Mesh* pMesh, N_RayHitResult& outHitRes);

		//TODO: ray-Mesh intersection. gpu impl. simply modify a little bit to Picking_GpuBased
		//bool IntersectRayMesh_GpuBased(const N_Ray& ray, Mesh* pMesh, N_HitResult& outHitRes);

	private:
		
		CollisionTestor();

		~CollisionTestor();

		friend IFactory<CollisionTestor>;

		friend SceneManager;

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init();

		//depth stencil state
		bool mFunction_InitDSS();

		static const UINT c_maxSOVertexCount = 200;//-------Var for Gpu Picking-----------

		IShaderVariableManager* m_pRefShaderVarMgr;

		ID3D11Buffer*			m_pSOGpuWriteableBuffer;
		ID3D11Buffer*			m_pSOCpuReadableBuffer;//this buffer will be used only when concrete collision point pos is needed
		ID3D11Query*			m_pSOQuery;//Inherited from ID3D11Async which is used to query SO information
		ID3DX11EffectTechnique*			m_pFX_Tech_Picking;//gpu acceleration picking intersection
		ID3D11DepthStencilState*			m_pDSS_DisableDepthTest;
	};
}