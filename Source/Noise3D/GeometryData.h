
/***********************************************************************

								 h£ºGeometry Data 
				desc: manage geometry data in sys mem & video mem

************************************************************************/
#pragma once

namespace Noise3D
{
	//manage vertex buffer and (possibly) index buffer as the same time
	template <typename vertex_t, typename index_t>
	class GeometryData
	{
	public:

		uint32_t	GetIndexCount();

		uint32_t	GetTriangleCount();

		void		GetVertex(index_t idx, vertex_t& outVertex);

		const	std::vector<vertex_t>*		GetVertexBuffer() const;

		const	std::vector<index_t>*		GetIndexBuffer() const;

		//WARNING!!!! bounding box is computed without applying a world transformation to vertices
		N_AABB	ComputeLocalAABB();

		// vertices are transformed before computing AABB, AffineTransform class is needed.
		//N_AABB	ComputeWorldAABB();

	private:

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_CreateGpuBufferAndUpdateData(const std::vector<vertex_t>& targetVB, const std::vector<index_t>& targetIB);

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_CreateGpuBufferAndUpdateData();

		ID3D11Buffer*		m_pVB_Gpu;
		ID3D11Buffer*		m_pIB_Gpu;
		std::vector<vertex_t> mVB_Mem;//vertex in CPU memory
		std::vector<index_t>	mIB_Mem;//index in CPU memory
	};

};
