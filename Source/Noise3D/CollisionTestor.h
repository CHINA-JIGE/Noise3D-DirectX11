
/***********************************************************************

								h£ºCollision Testor
				provide all kinds of collision detection
				between all kinds of objects

************************************************************************/

#pragma once

#include "_RayHitInfo.h"

namespace Noise3D
{
	class IShaderVariableManager;//for gpu-accelerated stuffs


	class /*_declspec(dllexport)*/ CollisionTestor
	{
	public:

		//normalized Coord frame(Cartesian) centered at the middle of screen ,with X & Y both valued in [-1,1]
		void Picking_GpuBased(Mesh* pMesh, const Vec2& mouseNormalizedCoord,std::vector<Vec3>& outCollidedPointList);

		//normalized Coord frame(Cartesian) centered at the middle of screen ,with X & Y both valued in [-1,1]
		UINT Picking_GpuBased(Mesh* pMesh, const Vec2& mouseNormalizedCoord);

		//ray-Aabb intersection. 'slabs' method, can refer to pbrt-v3 or peter-shirley's <Ray Tracing:The Next Week>
		static bool IntersectRayAabb(const N_Ray& ray, const N_AABB& aabb);

		//ray-Aabb intersection(detailed hit info). 'slabs' method, can refer to pbrt-v3 or peter-shirley's <Ray Tracing:The Next Week>
		static bool IntersectRayAabb(const N_Ray& ray, const N_AABB& aabb, N_RayHitResult& outHitRes);

		//ray-Rect intersection. the original rect lies on XY/XZ/YZ plane, calculation can be reduced
		static bool IntersectRayRect(const N_Ray& ray, LogicalRect* pRect, N_RayHitResult& outHitRes);

		//ray-Box intersection. box can be transformed in world space.
		static bool IntersectRayBox(const N_Ray& ray, LogicalBox* pBox);

		//ray-Box intersection. box can be transformed in world space.
		static bool IntersectRayBox(const N_Ray& ray, LogicalBox* pBox, N_RayHitResult& outHitRes);

		//ray-sphere intersecton. simply solve an quadratic equation
		static bool IntersectRaySphere(const N_Ray& ray, LogicalSphere* pSphere, N_RayHitResult& outHitRes);

		//ray-triangle intersection
		static bool IntersectRayTriangle(const N_Ray& ray, Vec3 v0, Vec3 v1, Vec3 v2, N_RayHitInfo& outHitInfo);

		//ray-triangle intersection with normal interpolation
		static bool IntersectRayTriangle(const N_Ray& ray, const N_DefaultVertex& v0, const N_DefaultVertex& v1, const N_DefaultVertex& v2, N_RayHitInfo& outHitInfo);

		//ray-Mesh intersection. cpu impl.
		static bool IntersectRayMesh(const N_Ray& ray, Mesh* pMesh, N_RayHitResult& outHitRes);

		//ray-Mesh intersection. gpu GS& stream output impl.
		bool IntersectRayMesh_GpuBased(const N_Ray& ray, Mesh* pMesh, N_RayHitResult& outHitRes);

		//remember to Rebuild BVH tree before intersectRayScene
		//ray-scene(all objects that attached to scene graph) intersection with BVH acceleration, O(logN) in average 
		bool IntersectRayScene(const N_Ray& ray,N_RayHitResult& outHitRes);

		//remember to Rebuild BVH tree before intersectRayScene
		//another version of ray-scene collision for path tracer
		bool IntersectRaySceneForPathTracer(const N_Ray& ray, N_RayHitResultForPathTracer& outHitRes);

		//(re-)build BVH tree from scene graph for ray tracer
		bool RebuildBvhTreeForGI(const SceneGraph& graph);

		//(re-)build BVH tree from scene graph for ray tracer, but rooted at given node
		bool RebuildBvhTreeForGI(SceneNode* pNode);

		const BvhTreeForGI& GetBvhTree();

		//TODO: ray-Mesh intersection. gpu impl. simply modify a little bit to Picking_GpuBased
		//bool IntersectRayMesh_GpuBased(const N_Ray& ray, Mesh* pMesh, N_HitResult& outHitRes);

	private:
		
		CollisionTestor();

		~CollisionTestor();

		friend IFactory<CollisionTestor>;

		friend SceneManager;

		//helper of transforming ray or result hit point
		//scene object that is attached to scene node can make use of this
		struct RayIntersectionTransformHelper
		{
			bool Ray_WorldToModel(const N_Ray& in_ray_world, bool isRigidTransform, ISceneObject * pObj, N_Ray& out_ray_local);

			void HitResult_ModelToWorld(N_RayHitResult& hitResult);

			Matrix worldMat; 
			Matrix worldInvMat; 
			Matrix worldInvTransposeMat;
		};

		//init d3d-related interface
		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_Init();

		//depth stencil state
		bool mFunction_InitDSS();

		//'gamma' for floating error in pbrt-v3 /scr/core/pbrt.h
		static float mFunc_Gamma(int n);

		//get facet id for Ray-AABB intersection
		static void mFunction_AabbFacet(uint32_t slabsPairId, float dirComponent, NOISE_BOX_FACET& nearHit, NOISE_BOX_FACET& farHit);

		//recursion function for BVH acceleration
		void mFunction_IntersectRayBvhNode(const N_Ray& ray, BvhNodeForGI* bvhNode, N_RayHitResult& outHitRes);

		//recursion function for BVH acceleration. extra info are added
		void mFunction_IntersectRayBvhNodeForPathTracer(const N_Ray& ray, BvhNodeForGI* bvhNode, N_RayHitResultForPathTracer& outHitRes);

		//update GPU states for GPU based intersection(ray-mesh/ picking)
		void mFunction_UpdateGpuInfoForRayIntersection(Mesh* pMesh, bool updateCamToGpu, bool updateMatrixToGpu);



		//low level render/shader variables support
		IShaderVariableManager* m_pRefShaderVarMgr;

		//for ray-scene intersection acceleration
		BvhTreeForGI mBvhTree;

		//-------Var for Gpu intersection-----------
		static const uint32_t c_maxSOVertexCount = 200;
		std::mutex				mSOMutex;//path tracer is multi-threaded
		ID3D11Buffer*			m_pSOGpuWriteableBuffer;
		ID3D11Buffer*			m_pSOCpuReadableBuffer;//this buffer will be used only when concrete collision point pos is needed
		ID3D11Query*			m_pSOQuery;//Inherited from ID3D11Async which is used to query SO information
		ID3DX11EffectTechnique*			m_pFX_Tech_Picking;//gpu acceleration picking intersection
		ID3DX11EffectTechnique*			m_pFX_Tech_RayMesh;//gpu acceleration ray-mesh intersection
		ID3D11DepthStencilState*			m_pDSS_DisableDepthTest;
	};
}