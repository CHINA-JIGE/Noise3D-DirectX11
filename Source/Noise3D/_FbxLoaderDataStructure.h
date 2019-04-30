
/***********************************************************************

							   h£ºFBX loader
		Desc: basic data structure of fbx loader/fbx info

************************************************************************/

#pragma once

namespace Noise3D
{
	struct N_MeshSubsetInfo;
	struct N_BasicLambertMaterialDesc;

	struct N_FbxTextureMapsInfo
	{
		std::string diffMapName;
		std::string diffMapFilePath;
		std::string normalMapName;
		std::string normalMapFilePath;
		std::string specMapName;
		std::string specMapFilePath;
		std::string emissiveMapName;
		std::string emissiveMapFilePath;
	};

	struct N_FbxMaterialInfo
	{
		std::string matName;
		N_BasicLambertMaterialDesc matBasicInfo;
		N_FbxTextureMapsInfo texMapInfo;//including loading path and names of texture maps
	};

	struct N_FbxMeshInfo
	{
		N_FbxMeshInfo() {}

		N_UID	name;
		std::vector<N_DefaultVertex> vertexBuffer;
		std::vector<uint32_t> indexBuffer;
		std::vector<N_MeshSubsetInfo> subsetList;
		std::vector<N_FbxMaterialInfo> matList;
		Vec3 pos;//world translation
		Vec3 scale;
		Vec3 rotation;
	};

	struct N_FbxSkeletonInfo
	{

	};

	struct N_FbxLoadingResult
	{
		N_FbxLoadingResult() {}

		std::vector<N_FbxMeshInfo>			meshDataList;
		std::vector<N_FbxSkeletonInfo>	skeletonList;
	};


	//*****************load fbx with GI/Physically-based ray-tracing material****************
	struct N_FbxPbrtTextureMapsInfo
	{
		std::string albedoMapName;
		std::string albedoMapFilePath;
		std::string roughnessMapName;
		std::string roughnessMapFilePath;
		//std::string normalMapName;
		//std::string normalMapFilePath;
		std::string metallicityMapName;
		std::string metallicityMapFilePath;
		std::string emissiveMapName;
		std::string emissiveMapFilePath;
	};

	struct N_FbxPbrtMaterialInfo
	{
		std::string matName;
		GI::N_AdvancedMatDesc matBasicInfo;
		N_FbxPbrtTextureMapsInfo texMapInfo;//including loading path and names of texture maps
	};

	struct N_FbxPbrtMeshInfo
	{
		N_FbxPbrtMeshInfo() {}

		N_UID	name;
		std::vector<N_DefaultVertex> vertexBuffer;
		std::vector<uint32_t> indexBuffer;
		std::vector<N_MeshSubsetInfo> subsetList;
		std::vector<N_FbxAdvancedMaterialInfo> matList;
		Vec3 pos;//world translation
		Vec3 scale;
		Vec3 rotation;
	};

	struct N_FbxPbrtSceneLoadingResult
	{
		N_FbxPbrtSceneLoadingResult() {}

		std::vector<N_FbxMeshInfo>	meshDataList;
	};

}