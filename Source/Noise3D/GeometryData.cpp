/**************************************************************

				Geometry Data (vertex& (perhaps)index)

				
	If i didn't enable VS's PCH, i shall add one more cpp file 
	for the seperation of c++ template's declaration and definition
	functions in a template class is just a template awaiting for instantiation/expansion
	instantiation need to be done explicitly
	https://blog.csdn.net/u012814856/article/details/84645963

	//explicit template instantiation
	//#include "GeometryData.cpp"
	//template Noise3D::GeometryData<N_DefaultVertex, uint32_t>;
	//template Noise3D::GeometryData<N_SimpleVertex, uint32_t>;


***********************************************************/
#include "Noise3D.h"
using namespace Noise3D;
using namespace Noise3D::D3D;

template<typename vertex_t, typename index_t>
inline uint32_t GeometryData<vertex_t, index_t>::GetIndexCount()
{
	return mIB_Mem.size();
}

template<typename vertex_t, typename index_t>
uint32_t Noise3D::GeometryData<vertex_t, index_t>::GetTriangleCount()
{
	return mIB_Mem.size() / 3;
}

template<typename vertex_t, typename index_t>
void Noise3D::GeometryData<vertex_t, index_t>::GetVertex(index_t idx, vertex_t& outVertex)
{
	if (idx < mVB_Mem.size())
	{
		outVertex = mVB_Mem.at(iIndex);
	}
}

template<typename vertex_t, typename index_t>
const std::vector<vertex_t>* Noise3D::GeometryData<vertex_t, index_t>::GetVertexBuffer() const
{
	return &mVB_Mem;
}

template<typename vertex_t, typename index_t>
const std::vector<index_t>* Noise3D::GeometryData<vertex_t, index_t>::GetIndexBuffer() const
{
	return &mIB_Mem;
}



/*********************************************

							PRIVATE

**********************************************/
template <typename vertex_t ,typename index_t>
bool NOISE_MACRO_FUNCTION_EXTERN_CALL Noise3D::GeometryData<typename vertex_t, typename index_t>::mFunction_CreateGpuBufferAndUpdateData(const std::vector<vertex_t>& targetVB, const std::vector<index_t>& targetIB)
{
	//check if buffers have been created
	ReleaseCOM(m_pVB_Gpu);
	ReleaseCOM(m_pIB_Gpu);

	//this function could be externally invoked by ModelLoader..etc
	mVB_Mem = targetVB;
	mIB_Mem = targetIB;

	//Prepare to update to video memory, fill in SUBRESOURCE description structure
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &mVB_Mem.at(0);
	uint32_t vertexCount = mVB_Mem.size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &mIB_Mem.at(0);
	index_t indexCount = mIB_Mem.size();

	//------Create VERTEX BUFFER
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(vertex_t)* vertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create Buffers
	HRESULT hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create vertex buffer ! ");


	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(index_t) * indexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	//Create Buffers
	hr = g_pd3dDevice11->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create index buffer ! ");

	return true;
}

template<typename vertex_t, typename index_t>
bool NOISE_MACRO_FUNCTION_EXTERN_CALL Noise3D::GeometryData<vertex_t, index_t>::mFunction_CreateGpuBufferAndUpdateData()
{
	ReleaseCOM(m_pVB_Gpu);
	ReleaseCOM(m_pIB_Gpu);

	//Prepare to update to video memory, fill in SUBRESOURCE description structure
	D3D11_SUBRESOURCE_DATA tmpInitData_Vertex;
	ZeroMemory(&tmpInitData_Vertex, sizeof(tmpInitData_Vertex));
	tmpInitData_Vertex.pSysMem = &mVB_Mem.at(0);
	UINT vertexCount = mVB_Mem.size();

	D3D11_SUBRESOURCE_DATA tmpInitData_Index;
	ZeroMemory(&tmpInitData_Index, sizeof(tmpInitData_Index));
	tmpInitData_Index.pSysMem = &mIB_Mem.at(0);
	UINT indexCount = mIB_Mem.size();

	//------create vertex buffer-------
	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(N_DefaultVertex)* vertexCount;
	vbd.Usage = D3D11_USAGE_DEFAULT;//_DEFAULT:GPU-read-write; _IMMUTABLE:
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; //CPU do nothing. D3D11_CPU_ACCESS_FLAG_XXX
	vbd.MiscFlags = 0;//D3D11_RESOURCE_MISC_RESOURCE. refer to MSDN for more info
	vbd.StructureByteStride = 0;
	HRESULT hr = 0;
	hr = g_pd3dDevice11->CreateBuffer(&vbd, &tmpInitData_Vertex, &m_pVB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create vertex buffer ! ");

	//------create index buffer------
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(int) * indexCount;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	hr = g_pd3dDevice11->CreateBuffer(&ibd, &tmpInitData_Index, &m_pIB_Gpu);
	HR_DEBUG(hr, "Mesh : Failed to create index buffer ! ");

	return true;
}
