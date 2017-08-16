/***********************************************************************

                           h£ºOBJ file import/export

************************************************************************/

#pragma once

namespace Noise3D
{
	//in OBJ file ,vertex info is composed of indices
	struct N_LoadOBJ_vertexInfoIndex
	{
		N_LoadOBJ_vertexInfoIndex()
		{
			vertexID = texcoordID = vertexNormalID = 0;
		};

		N_LoadOBJ_vertexInfoIndex(int vID, int texcID, int vnID)
		{
			vertexID = vID;
			texcoordID = texcID;
			vertexNormalID = vnID;
		}

		inline bool operator==(N_LoadOBJ_vertexInfoIndex const& v)const
		{
			if (vertexID == v.vertexID && texcoordID == v.texcoordID && vertexNormalID == v.vertexNormalID)
			{
				return true;
			}
			return false;
		}

		UINT vertexID;
		UINT texcoordID;
		UINT vertexNormalID;
	};

	class IFileIO_OBJ
	{
	public:

		bool ImportFile_OBJ(NFilePath pFilePath, std::vector<N_DefaultVertex>& outVertexBuffer, std::vector<UINT>& outIndexBuffer);

	private:

	};



};
