
/***********************************************************************

						h£ºSweeping Trail Manager

		Desc: Sweeping Trail is an special effect that uses
		a sequence of triangle strip to simulate/visualize a
		path that a line segment sweep through 3D spaces.
		**Some example application scenerio: a melee weapon
		swinging in the air, following by a colored 'trail'.

		picture ref:https://blog.csdn.net/cbbbc/article/details/70863580

		you can "SetHeader" every frame to indicate the head of trail,
		and the trail quad sequence will automatically generate as long as 
		you call "Update(dt)" every frame.
************************************************************************/

#pragma once

namespace Noise3D
{

	class SweepingTrail:
		public ISceneObject,
		public CRenderSettingBlendMode,
		public CRenderSettingFillMode
	{
	public:

		//set current header line segments (in local space)
		//(2019.3.11)Noise3D will automatically apply rigid transform to the header before updating/rendering
		void SetHeader(N_LineSegment lineSeg);

		//header segment(world space)
		N_LineSegment GetHeader();

		//the center of the header segment(world space)
		NVECTOR3 GetHeaderCenterPos();

		//time limit of cooling down the header line segment and GENERATE a new "free" header
		//unit: Milli-second
		void SetHeaderCoolDownTimeThreshold(float duration_ms);

		//!!!!Every line segment has its life time.
		//(which means, the last LS might not approach to second last LS at the beginning)
		//(which also means, the last line segment's texcoord.u might be much less than 1 for a while)
		//unit: Milli-second
		void SetMaxLifeTimeOfLineSegment(float duration_ms);

		//vertice count generated this frame(updated after calling Update(dt))
		uint32_t GetActiveVerticesCount();

		//Cubic Hermite interpolation steps (count of sub-regions divided into)
		void SetInterpolationStepCount(uint32_t count);

		//Cubic Hermite interpolation steps of the free header quad (count of sub-regions divided into)
		void SetFreeHeaderInterpolationStepCount(uint32_t count);

		//set the length of Cubic Hermite Interpolation's tangent scale (normally [0,1])
		void SetCubicHermiteTangentScale(float scale);

		//updating vertices (to gpu vertex buffer):
		//1. header line segment's position 
		//2. tail line segments position
		//3. every vertices' uv (actually only texcoord.u changes)
		void Update(float deltaTime);

		void SetTextureName(N_UID texName);

		N_UID GetTextureName();

		//for debug use. Copy the whole list to outside. This might cause performance overhead, plz be noticed.
		void GetTangentList(std::vector<std::pair<NVECTOR3, NVECTOR3>>& outList);

		void GetVerticesList(std::vector<Noise3D::N_LineSegment>& outList);

		//ISceneObject::
		virtual N_AABB GetLocalAABB() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Accurate() override;

		//ISceneObject::
		virtual N_AABB ComputeWorldAABB_Fast();

		//ISceneObject::
		virtual NOISE_SCENE_OBJECT_TYPE GetObjectType() const override;

	private:

		//curved interpolation is needed, use tangent for Cubic Hermite
		struct N_GenQuadInfo
		{
			N_GenQuadInfo() :interpolation_steps(1), collapsingFactor(0.0f){};

			uint32_t interpolation_steps;//final sub-regions count. e.g. 5 steps, each 0.2 lerp ratio, 4 cuts
			float collapsingFactor;//for tail quad, mTailCollapsingFactor should be passed in
			NVECTOR3 frontPos1;
			NVECTOR3 frontPos2;
			NVECTOR3 frontTangent1;//estimated tangent (for Cubic Hermite Spline)
			NVECTOR3 frontTangent2;//estimated tangent (for Cubic Hermite Spline)
			NVECTOR3 backPos1;
			NVECTOR3 backPos2;
			NVECTOR3 backTangent1;//estimated tangent (for Cubic Hermite Spline)
			NVECTOR3 backTangent2;//estimated tangent (for Cubic Hermite Spline)
		};

		friend class SweepingTrailManager;
		friend IFactory<SweepingTrail>;
		friend class IRenderModuleForSweepingTrailFX;
		typedef N_SimpleVertex N_SweepingTrailVertexType;

		SweepingTrail();

		~SweepingTrail();

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGpuBuffer(UINT vertexPoolSize);

		// CoolingDown
		void mFunction_CoolDownHeader();

		//Collapsing & Movement
		void mFunction_MoveAndCollapseTail();

		//estimate tangent of given point
		void mFunction_EstimateTangents();

		//Gen actually vertices to draw
		void mFunction_GenVerticesAndUpdateToGpuBuffer();

		//calculate given line segment's life timer
		float mFunction_UtComputeLifeTimer(int lineSegmentIndex);

		//generated interpolated quad group(between a pair of line segment)
		int mFunction_UtGenQuad(const N_GenQuadInfo& desc, float frontLifeTimer, float backLifeTimer, N_SweepingTrailVertexType* quad);	//return vertices generated

		//seperating header and tail from the middle fixed line segment brings a lot of troubling corner cases...
		N_LineSegment mFunction_UtGetLineSegment(int index);

		//...
		ID3D11Buffer*	 m_pVB_Gpu;//(2018.7.23)simple vertex
		uint32_t mGpuVertexPoolByteCapacity;
		uint32_t mGpuPoolMaxVertexCount;

		//line segments list (free/dynamic HEADER line segments are EXCLUDED)
		std::vector<Noise3D::N_LineSegment> mFixedLineSegments;
		N_LineSegment mFreeHeader;//keep updating by "SetHeader" until certain 'cool down time' is reached
		//N_LineSegment mFreeHeader_PreviousState;//store previous free header LS to optimize header's tangent
		N_LineSegment mFreeTail_Start;//the tail LS keep approaching to the second last LS. And the lerp start should be saved.
		float mTailQuadCollapsingRatio;//[0,1] ratio for the line segment vertex to lerp from mFreeTail_Start to mFreeTail_Current
		uint32_t mInterpolationStepCount;//steps for cubic hermite interpolation
		uint32_t mFreeHeaderInterpStepCount;
		float mCubicHermiteTangentScale;//scale the tangent length for cubic hermite interp
		uint32_t mGeneratedVerticesCount;//updated after each "update" call

		//Tangents for Hermite Interpolation
		std::vector< std::pair<NVECTOR3, NVECTOR3>> mTangentList;
		NVECTOR3 mFreeTailTangent1;		//store old tangents(calculated for the 2nd last LS previously) to prevent that the estimated tangent changes suddenly
		NVECTOR3 mFreeTailTangent2;

		float mHeaderCoolDownTimeThreshold;//after given time, the header segment will be fixed down and add to "Cooled down line segments"
		float mHeaderCoolDownTimer;	//timer (initially 0, increment)
		float mMaxLifeTimeOfLS;//the maximum time each line segment can live

		//Texture
		N_UID mTextureUid;

	};
}