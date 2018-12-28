
/***********************************************************************

								 h£ºHardwareVertexBuffer
				desc: encapsule common info's of a render pass

************************************************************************/
#pragma once

namespace Noise3D
{
	//manage vertex buffer and (possibly) index buffer as the same time
	class HardwareBuffer
	{
	public:

		UINT	GetIndexCount();

		UINT	GetTriangleCount();

		void		GetVertex(UINT iIndex, N_DefaultVertex& outVertex);

		const	std::vector<N_DefaultVertex>*	GetVertexBuffer() const;

		const	std::vector<UINT>*		GetIndexBuffer() const;

		//WARNING!!!! bounding box is computed without applying a world transformation to vertices
		N_Box	ComputeBoundingBox();

	private:

		//this function use the vertex list of vector<N_DefaultVertex>
		void		mFunction_ComputeBoundingBox();

		void		mFunction_ComputeBoundingBox(std::vector<NVECTOR3>* pVertexBuffer);

		ID3D11Buffer*		m_pVB_Gpu;
		ID3D11Buffer*		m_pIB_Gpu;
		std::vector<N_DefaultVertex> mVB_Mem;//vertex in CPU memory
		std::vector<UINT>	mIB_Mem;//index in CPU memory
		N_Box					mBoundingBox;
	};
}
