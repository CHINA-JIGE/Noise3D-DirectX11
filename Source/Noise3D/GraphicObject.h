
/***********************************************************************

					h：NoiseGraphicObject
				主要功能 ：添加各种线

************************************************************************/
#pragma once

namespace Noise3D
{
	enum NOISE_GRAPHIC_OBJECT_TYPE
	{
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_3D = 0,
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_3D = 1,
		NOISE_GRAPHIC_OBJECT_TYPE_POINT_2D = 2,
		NOISE_GRAPHIC_OBJECT_TYPE_LINE_2D = 3,
		NOISE_GRAPHIC_OBJECT_TYPE_TRIANGLE_2D = 4,
		NOISE_GRAPHIC_OBJECT_TYPE_RECT_2D = 5,
		NOISE_GRAPHIC_OBJECT_BUFFER_COUNT,
	};

	struct N_GraphicObject_SubsetInfo
	{
		N_GraphicObject_SubsetInfo() {startID=vertexCount=0; }
		N_UID texName;
		UINT startID;
		UINT vertexCount;
	};

	class /*_declspec(dllexport)*/ IGraphicObject
		: public CRenderSettingBlendMode
	{

	public:

		void		SetBasePosOffset(NVECTOR2 pixelOffset);

		NVECTOR2 GetBasePosOffset();

		UINT	AddLine3D(NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		UINT	AddLine2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		UINT	AddPoint3D(NVECTOR3 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		UINT	AddPoint2D(NVECTOR2 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		UINT	AddTriangle2D(NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color3 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		UINT	AddRectangle(NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color, const N_UID& texName = "");

		UINT	AddRectangle(NVECTOR2 vCenter, float fWidth, float fHeight, NVECTOR4 color, const N_UID& texName = "");

		void		AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE objType, UINT newElementCount);

		void		SetLine3D(UINT index, NVECTOR3 v1, NVECTOR3 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetLine2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetPoint3D(UINT index, NVECTOR3 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetPoint2D(UINT index, NVECTOR2 v, NVECTOR4 color = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetTriangle2D(UINT index, NVECTOR2 v1, NVECTOR2 v2, NVECTOR2 v3, NVECTOR4 color1 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color2 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), NVECTOR4 color3 = NVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetRectangle(UINT index, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight, NVECTOR4 color, const N_UID& texName);

		void		SetRectangle(UINT index, NVECTOR2 vCenter, float fWidth, float fHeight, NVECTOR4 color, const N_UID& texName);

		void		SetRectangleTexCoord(UINT index, NVECTOR2 texCoordTopLeft, NVECTOR2 texCoordBottomRight);

		void		SetRectangleDepth(UINT index, float posZ);

		void		DeleteLine3D(UINT index);

		void		DeleteLine3D(UINT startID, UINT endID);

		void		DeleteLine2D(UINT index);

		void		DeleteLine2D(UINT startID, UINT endID);

		void		DeletePoint3D(UINT index);

		void		DeletePoint3D(UINT startID, UINT endID);

		void		DeletePoint2D(UINT index);

		void		DeletePoint2D(UINT startID, UINT endID);

		void		DeleteTriangle2D(UINT index);

		void		DeleteTriangle2D(UINT startID, UINT endID);

		void		DeleteRectangle(UINT index);

		void		DeleteRectangle(UINT startID, UINT endID);

		UINT		GetLine3DCount();

		UINT		GetLine2DCount();

		UINT		GetPoint3DCount();

		UINT		GetPoint2DCount();

		UINT		GetTriangle2DCount();

		UINT		GetRectCount();

	private:

		friend  class IRenderModuleForGraphicObject;

		friend class IRenderModuleForText;//it's a little bit ugly...but GraphicObject serves as a container for text graphics

		friend IFactory<IGraphicObject>;

		IGraphicObject();

		~IGraphicObject();

		bool			mFunction_CreateVB(UINT objType_ID);

		void			NOISE_MACRO_FUNCTION_EXTERN_CALL 	mFunction_UpdateVerticesToGpu(UINT objType_ID);//by renderer

		//set/add/delete  using initializer_list
		void			mFunction_AddVertices2D(NOISE_GRAPHIC_OBJECT_TYPE buffType, std::initializer_list<NVECTOR2> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList);

		void			mFunction_AddVertices3D(NOISE_GRAPHIC_OBJECT_TYPE buffType, std::initializer_list<NVECTOR3> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList);

		void			mFunction_SetVertices2D(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, std::initializer_list<NVECTOR2> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList);

		void			mFunction_SetVertices3D(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, std::initializer_list<NVECTOR3> vertexList, std::initializer_list<NVECTOR4> colorList, std::initializer_list<NVECTOR2> texcoordList);

		void			mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, UINT iVertexCount);

		void			mFunction_ConvertFloatVec2PixelVec(NVECTOR2& vec);

		void			mFunction_ConvertPixelVec2FloatVec(NVECTOR2& in_out_vec);

		float			mFunction_ConvertPixelLength2FloatLength(float pxLen, bool isWidth);

		void			mFunction_AdjustElementCount(UINT newCount, UINT currentObjCount, NOISE_GRAPHIC_OBJECT_TYPE objType);

		void			NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_GenerateRectSubsetInfo();//for optimization,by renderer

	private:

		UINT					mVB_ByteSize_GPU[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		ID3D11Buffer*		m_pVB_GPU[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		bool						mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		std::vector<N_SimpleVertex>*	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		NVECTOR2*			m_pBaseScreenSpacePosOffset;

		//used to store TexID of rectangles
		std::vector<N_UID>*			m_pTextureUidList_Rect;

		//this subset list is generated for renderer
		std::vector<N_GraphicObject_SubsetInfo>*	m_pRectSubsetInfoList;

	};
}