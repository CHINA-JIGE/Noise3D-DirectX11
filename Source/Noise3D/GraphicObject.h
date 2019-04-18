
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

	class /*_declspec(dllexport)*/ GraphicObject
		: public ISceneObject,
		public CRenderSettingBlendMode
	{

	public:

		void		SetBasePosOffset(Vec2 pixelOffset);

		Vec2 GetBasePosOffset();

		uint32_t AddLine3D(Vec3 v1, Vec3 v2, Vec4 color1 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color2 = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		uint32_t AddLine3D_AABB(Vec3 min, Vec3 max, Vec4 color1 = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		uint32_t AddLine2D(Vec2 v1, Vec2 v2, Vec4 color1 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color2 = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		uint32_t AddPoint3D(Vec3 v, Vec4 color = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		uint32_t AddPoint2D(Vec2 v, Vec4 color = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		uint32_t AddTriangle2D(Vec2 v1, Vec2 v2, Vec2 v3, Vec4 color1 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color2 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color3 = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		uint32_t AddRectangle(Vec2 vTopLeft, Vec2 vBottomRight, Vec4 color, const N_UID& texName = "");

		uint32_t AddRectangle(Vec2 vCenter, float fWidth, float fHeight, Vec4 color, const N_UID& texName = "");

		void		AdjustElementCount(NOISE_GRAPHIC_OBJECT_TYPE objType, UINT newElementCount);

		void		SetLine3D(UINT index, Vec3 v1, Vec3 v2, Vec4 color1 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color2 = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetLine2D(UINT index, Vec2 v1, Vec2 v2, Vec4 color1 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color2 = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetPoint3D(UINT index, Vec3 v, Vec4 color = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetPoint2D(UINT index, Vec2 v, Vec4 color = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetTriangle2D(UINT index, Vec2 v1, Vec2 v2, Vec2 v3, Vec4 color1 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color2 = Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4 color3 = Vec4(1.0f, 1.0f, 1.0f, 1.0f));

		void		SetRectangle(UINT index, Vec2 vTopLeft, Vec2 vBottomRight, Vec4 color, const N_UID& texName);

		void		SetRectangle(UINT index, Vec2 vCenter, float fWidth, float fHeight, Vec4 color, const N_UID& texName);

		void		SetRectangleTexCoord(UINT index, Vec2 texCoordTopLeft, Vec2 texCoordBottomRight);

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

		//SceneNode::
		AffineTransform& GetLocalTransform() = delete;

		//SceneNode::
		Matrix EvalWorldAffineTransformMatrix() = delete;

		//SceneNode::
		Matrix EvalWorldRigidTransformMatrix() = delete;

		//SceneNode::
		Matrix EvalWorldRotationMatrix() = delete;

		//SceneNode::
		void EvalWorldAffineTransformMatrix(Matrix& outWorldMat, Matrix& outWorldInvTranspose) = delete;

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Fast() override;

		//ISceneObject::
		virtual N_BoundingSphere ComputeWorldBoundingSphere_Accurate() override;

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType() const  override;

	private:

		friend  class IRenderModuleForGraphicObject;

		friend class IRenderModuleForText;//it's a little bit ugly...but GraphicObject serves as a container for text graphics

		friend IFactory<GraphicObject>;

		GraphicObject();

		~GraphicObject();

		bool			mFunction_CreateVB(UINT objType_ID);

		void			NOISE_MACRO_FUNCTION_EXTERN_CALL 	mFunction_UpdateVerticesToGpu(UINT objType_ID);//by renderer

		//set/add/delete  using initializer_list
		void			mFunction_AddVertices2D(NOISE_GRAPHIC_OBJECT_TYPE buffType, std::initializer_list<Vec2> vertexList, std::initializer_list<Vec4> colorList, std::initializer_list<Vec2> texcoordList);

		void			mFunction_AddVertices3D(NOISE_GRAPHIC_OBJECT_TYPE buffType, std::initializer_list<Vec3> vertexList, std::initializer_list<Vec4> colorList, std::initializer_list<Vec2> texcoordList);

		void			mFunction_SetVertices2D(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, std::initializer_list<Vec2> vertexList, std::initializer_list<Vec4> colorList, std::initializer_list<Vec2> texcoordList);

		void			mFunction_SetVertices3D(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, std::initializer_list<Vec3> vertexList, std::initializer_list<Vec4> colorList, std::initializer_list<Vec2> texcoordList);

		void			mFunction_EraseVertices(NOISE_GRAPHIC_OBJECT_TYPE buffType, UINT iVertexStartID, UINT iVertexCount);

		void			mFunction_ConvertFloatVec2PixelVec(Vec2& vec);

		void			mFunction_ConvertPixelVec2FloatVec(Vec2& in_out_vec);

		float			mFunction_ConvertPixelLength2FloatLength(float pxLen, bool isWidth);

		void			mFunction_AdjustElementCount(UINT newCount, UINT currentObjCount, NOISE_GRAPHIC_OBJECT_TYPE objType);

		void			NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_GenerateRectSubsetInfo();//for optimization,by renderer

	private:

		UINT					mVB_ByteSize_GPU[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		ID3D11Buffer*		m_pVB_GPU[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		bool						mCanUpdateToGpu[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		std::vector<N_SimpleVertex>*	m_pVB_Mem[NOISE_GRAPHIC_OBJECT_BUFFER_COUNT];

		Vec2*			m_pBaseScreenSpacePosOffset;

		//used to store TexID of rectangles
		std::vector<N_UID>*			m_pTextureUidList_Rect;

		//this subset list is generated for renderer
		std::vector<N_GraphicObject_SubsetInfo>*	m_pRectSubsetInfoList;

	};
}