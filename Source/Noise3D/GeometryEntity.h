
/***********************************************************************

								 h£ºGeometry Data 
				desc: manage geometry data in sys mem & video mem

************************************************************************/
#pragma once

namespace Noise3D
{
	//class inherited from this base interface can be attach to scene node.
	class ISceneObject
	{
	public:
		virtual N_AABB ComputeWorldAABB() = 0;
	};


	//base class/interface of a geometry entity (with actual vertex/index data)
	//manage vertex buffer and (possibly) index buffer as the same time
	template <typename vertex_t, typename index_t>
	class GeometryEntity:
		public ISceneObject
	{
	public:

		GeometryEntity();

		uint32_t	GetIndexCount();

		uint32_t	GetTriangleCount();

		void		GetVertex(index_t idx, vertex_t& outVertex);

		const	std::vector<vertex_t>*		GetVertexBuffer() const;

		const	std::vector<index_t>*		GetIndexBuffer() const;

		//compute bounding box without applying a world transformation to vertices(local space)
		N_AABB	GetLocalAABB();

		// interface. AffineTransform class is needed. might be implemented in inherited class like 'SceneNode'
		//(SceneNode would have information's about AffineTransformation)
		virtual N_AABB ComputeWorldAABB() override = 0;

	private:

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_CreateGpuBufferAndUpdateData(const std::vector<vertex_t>& targetVB, const std::vector<index_t>& targetIB);

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_CreateGpuBufferAndUpdateData();

		ID3D11Buffer*		m_pVB_Gpu;
		ID3D11Buffer*		m_pIB_Gpu;
		std::vector<vertex_t> mVB_Mem;//vertex in CPU memory
		std::vector<index_t>	mIB_Mem;//index in CPU memory
		bool						mIsLocalAabbInitialized;
		N_AABB				mBoundingBox;//local AABB is calculated only once(and is the minimum AABB)
	};

};
