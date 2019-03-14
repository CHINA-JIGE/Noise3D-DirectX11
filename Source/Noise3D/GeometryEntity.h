
/***********************************************************************

								 h£ºGeometry Entity 
				desc: manage geometry data in sys mem & video mem

************************************************************************/
#pragma once

namespace Noise3D
{

	enum NOISE_SCENE_OBJECT_TYPE;//defined in other header

	//base class/interface of a geometry entity (with actual vertex/index data)
	//manage vertex buffer and (possibly) index buffer as the same time
	template <typename vertex_t, typename index_t>
	class GeometryEntity:
		public ISceneObject
	{
	public:

		GeometryEntity();

		virtual ~GeometryEntity();

		uint32_t	GetIndexCount();

		uint32_t	GetTriangleCount();

		void		GetVertex(index_t idx, vertex_t& outVertex);

		const	std::vector<vertex_t>*		GetVertexBuffer() const;

		const	std::vector<index_t>*		GetIndexBuffer() const;


		//compute bounding box without applying a world transformation to vertices(local space)
		virtual N_AABB GetLocalAABB() override;

		// AffineTransform is needed. 
		//need to retrieve transform infos from actual scene object. not implementing here
		virtual N_AABB ComputeWorldAABB_Accurate() override=0;

		//will be implemented in actual/concrete scene object class
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType() override = 0;

	protected:

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_CreateGpuBufferAndUpdateData(const std::vector<vertex_t>& targetVB, const std::vector<index_t>& targetIB);

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_CreateGpuBufferAndUpdateData();

		ID3D11Buffer*		m_pVB_Gpu;
		ID3D11Buffer*		m_pIB_Gpu;
		std::vector<vertex_t>	 mVB_Mem;//vertex in CPU memory
		std::vector<index_t>	 mIB_Mem;//index in CPU memory
		bool						mIsLocalAabbInitialized;
		N_AABB				mLocalBoundingBox;//local AABB is calculated only once(and is the minimum AABB)
	};

};
