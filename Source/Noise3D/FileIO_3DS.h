
/*******************************************************************

								.3DS LOADER (deprecated)

*********************************************************************/
#pragma once

namespace Noise3D
{
	struct N_MaterialDesc;
	struct N_MeshSubsetInfo;

	enum NOISE_3DS_CHUNKID
	{
		//some of these chunks are not interested
		NOISE_3DS_CHUNKID_MAIN3D = 0x4D4D,
		NOISE_3DS_CHUNKID_3D_EDITOR_CHUNK = 0x3D3D,
		NOISE_3DS_CHUNKID_OBJECT_BLOCK = 0x4000,
		NOISE_3DS_CHUNKID_TRIANGULAR_MESH = 0x4100,
		NOISE_3DS_CHUNKID_VERTICES_LIST = 0x4110,
		NOISE_3DS_CHUNKID_FACES_DESCRIPTION = 0x4120,
		NOISE_3DS_CHUNKID_FACES_MATERIAL = 0x4130,
		NOISE_3DS_CHUNKID_MAPPING_COORDINATES_LIST = 0x4140,
		NOISE_3DS_CHUNKID_SMOOTHING_GROUP_LIST = 0x4150,
		NOISE_3DS_CHUNKID_LOCAL_COORDINATES_SYSTEM = 0x4160,
		NOISE_3DS_CHUNKID_LIGHT = 0x4600,
		NOISE_3DS_CHUNKID_SPOTLIGHT = 0x4610,
		NOISE_3DS_CHUNKID_CAMERA = 0x4700,
		NOISE_3DS_CHUNKID_MATERIAL_BLOCK = 0xAFFF,
		NOISE_3DS_CHUNKID_MATERIAL_NAME = 0xA000,
		NOISE_3DS_CHUNKID_AMBIENT_COLOR = 0xA010,
		NOISE_3DS_CHUNKID_DIFFUSE_COLOR = 0xA020,
		NOISE_3DS_CHUNKID_SPECULAR_COLOR = 0xA030,
		NOISE_3DS_CHUNKID_TEXTURE_MAP = 0xA200,
		NOISE_3DS_CHUNKID_BUMP_MAP = 0xA230,
		NOISE_3DS_CHUNKID_SPECULAR_MAP = 0xA204,
		NOISE_3DS_CHUNKID_REFLECTION_MAP = 0xA220,
		NOISE_3DS_CHUNKID_MAPPING_FILENAME = 0xA300,
		NOISE_3DS_CHUNKID_MAPPING_PARAMETERS = 0xA351,

		// Common chunks which can be found everywhere in the file
		NOISE_3DS_CHUNKID_VERSION = 0x0002,
		NOISE_3DS_CHUNKID_RGBF = 0x0010,	// float4 R; float4 G; float4 B
		NOISE_3DS_CHUNKID_RGBB = 0x0011,	// int1 R; int1 G; int B
		NOISE_3DS_CHUNKID_LINRGBF = 0x0013,	// float4 R; float4 G; float4 B
		NOISE_3DS_CHUNKID_LINRGBB = 0x0012,	// int1 R; int1 G; int B
		NOISE_3DS_CHUNKID_PERCENTW = 0x0030,	// int2   percentage
		NOISE_3DS_CHUNKID_PERCENTF = 0x0031,	// float4  percentage
	};

	struct N_Load3ds_TexName_FilePath_Mapping
	{
		std::string textureName;
		std::string FilePath;
	};

	struct N_Load3ds_MeshObject
	{
		std::string meshName;
		std::vector<NVECTOR3> verticesList;
		std::vector<NVECTOR2> texcoordList;
		std::vector<UINT> indicesList;
		std::vector<N_MeshSubsetInfo>subsetList;
	};


	class /*_declspec(dllexport)*/ IFileIO_3DS
	{
	public:

		IFileIO_3DS();

		bool ImportFile_3DS(
			NFilePath pFilePath,
			std::vector<N_Load3ds_MeshObject>& outMeshInfoList,
			std::vector<N_MaterialDesc>& outMaterialList,
			std::vector<std::string>& outMatNameList,
			std::unordered_map<std::string, NFilePath>& out_TexName2FilePathPairList);

	private:

		std::string DecorateMatName(std::string name);

		void ReadChunk();

		void	SkipCurrentChunk();

		void	ReadStringFromFileA(std::string& outString);

		void	ReadStringFromFileW(std::wstring& outString);

		//void	IntegratePartialMeshIntoOnePiece(Noise3D::N_Load3ds_MeshObject& outCombinedMesh);

		//**********************************************
		void		ParseMainChunk();//1

		void			Parse3DEditorChunk();//2

		void				ParseObjectBlock();//3

		void					ParseTriangularMeshChunk();//4

		void						ParseVerticesChunk();//5

		void						ParseFaceDescAndIndices();//5

		void							ParseFacesAndMatName();//6

		void						ParseTextureCoordinate();//5

		void			ParseMaterialBlock();//3

		void				ParseMaterialName();//4

		void				ParseAmbientColor();//4

		void				ParseDiffuseColor();//4

		void				ParseSpecularColor();//4

		void					ReadAndParseColorChunk(Noise3D::NVECTOR3& outColor);//level 5 for each color chunk

		void				ParseDiffuseMap();//4

		void				ParseBumpMap();//4

		void				ParseSpecularMap();//4

		void					 ReadAndParseMapChunk(std::string& outGeneratedTextureName);//level 5 for each map chunk


	private:

		std::vector<N_Load3ds_MeshObject>*				m_pMeshObjList;		//Material Lists
		std::vector<N_MaterialDesc>*							m_pMaterialList;
		std::vector<std::string>*										m_pMatNameList;
		std::unordered_map<std::string, NFilePath>*	m_pTexName2FilePathPairList;	//string as UID, FilePath is used to load file

		static const UINT c_chunkHeadByteLength;

		static const std::string c_defaultTexNamePrefix;

		static const std::string c_defaultMatNamePrefix;

		static UINT		mMeshIndex;

		static UINT		mTextureMapIndex;

	};
};

/***********************************************************************
Offset			Length			Description
0					2			Chunk	identifier
2					4			Chunk	length : chunk data + sub - chunks(6 + n + m)
6					n			Data
6 + n			m			Sub-chunks
*********************************************************************/

/***************************************************

Here are hierarchies of some chunks;
some chunks don't have data, only sub-chunks;
We just read interested chunks, then skip the rest.
Switch(chunkID) to decide whether to read.

MAIN CHUNK 0x4D4D
   3D EDITOR CHUNK 0x3D3D
      OBJECT BLOCK 0x4000
         TRIANGULAR MESH 0x4100
            VERTICES LIST 0x4110
            FACES DESCRIPTION 0x4120
               FACES MATERIAL 0x4130
            MAPPING COORDINATES LIST 0x4140
               SMOOTHING GROUP LIST 0x4150
            LOCAL COORDINATES SYSTEM 0x4160
         LIGHT 0x4600
            SPOTLIGHT 0x4610
         CAMERA 0x4700
      MATERIAL BLOCK 0xAFFF
         MATERIAL NAME 0xA000
         AMBIENT COLOR 0xA010
         DIFFUSE COLOR 0xA020
         SPECULAR COLOR 0xA030
         TEXTURE MAP 1 0xA200
         BUMP MAP 0xA230
         REFLECTION MAP 0xA220
         [SUB CHUNKS FOR EACH MAP]
            MAPPING FILENAME 0xA300
            MAPPING PARAMETERS 0xA351
      KEYFRAMER CHUNK 0xB000
         MESH INFORMATION BLOCK 0xB002
         SPOT LIGHT INFORMATION BLOCK 0xB007
         FRAMES (START AND END) 0xB008
            OBJECT NAME 0xB010
            OBJECT PIVOT POINT 0xB013
            POSITION TRACK 0xB020
            ROTATION TRACK 0xB021
            SCALE TRACK 0xB022
            HIERARCHY POSITION 0xB030
************************************************************/

/************************CHUNK DETAILS - FROM BLOG*****************************
http://anony3721.blog.163.com/blog/static/5119742011525103920153/

0x4D4D：根chunk，每一个3ds文件都起自它，它的长度也就是文件的长度。它包含了两个chunk：编辑器，和关键帧。
父chunk：无
子chunk：0x3D3D、0xB000
长度：头长度+子chunk长度
内容：无

0x3D3D：编辑器主chunk，它包含有：网格信息、灯光信息、摄象机信息和材质信息。
父chunk：0x4D4D
子chunk：0x4000、0xafff
长度：头长度+子chunk长度
内容：无

0x4000：网格主chunk，它包含了所有的网格。
父chunk：0x3D3D
子chunk：0x4100
长度：头长度+子chunk长度+内容长度
内容：
名称（以空字节结尾的字符串）

0x4100：网格信息，包含网格名称、顶点、面、纹理坐标等。
父chunk：0x4000
子chunk：0x4110、0x4120、0x4140、0x4160
长度：头长度+子chunk长度
内容：无

0x4110：顶点信息。
父chunk：0x4100
子chunk：无
长度：头长度+内容长度
内容：
顶点个数（一个字）
顶点坐标（三个浮点数一个坐标x、y、z，个数*3*浮点数）

0x4120：面信息。
父chunk：0x4100
子chunk：0x4130
长度：头长度+子chunk长度+内容长度
内容：
面个数（一个字）
顶点索引（三个字一个索引1、2、3，个数*3*字）

0x4130：与网格相关的材质信息。
父chunk：0x4120
子chunk：无
长度：头长度+内容长度
内容：
名称（以空字节结尾的字符串）
与材质相连的面的个数（一个字）
与材质相连的面的索引（个数*字）

0x4140：纹理坐标。
父chunk：0x4100
子chunk：无
长度：头长度+内容长度
内容：
坐标个数（一个字）
坐标（两个浮点数一个坐标u、v，个数*2*浮点数）

0x4160：转换矩阵。
父chunk：0x4100
子chunk：无
长度：头长度+内容长度
内容：
x轴的向量（三个浮点数u、v、n）
y轴的向量（三个浮点数u、v、n）
z轴的向量（三个浮点数u、v、n）
源点坐标（三个浮点数x、y、z）

0xafff：材质信息。
父chunk：0x4D4D
子chunk：0xa000、0xa020、0xa200
长度：头长度+子chunk长度
内容：无

0xa000：材质名称。
父chunk：0xafff
子chunk：无
长度：头长度+内容长度
内容：
名称（以空字节结尾的字符串）


0xa020：Diffuse。
父chunk：0xafff
子chunk：0x0011、0x0012
长度：头长度+子chunk长度
内容：无


0xa200：纹理帖图。
父chunk：0xafff
子chunk：0xa300
长度：头长度+子chunk长度
内容：无

0xa300：贴图名称。
父chunk：0xa200
子chunk：无
长度：头长度+内容长度
内容：
名称（以空字节结尾的字符串）

*/
