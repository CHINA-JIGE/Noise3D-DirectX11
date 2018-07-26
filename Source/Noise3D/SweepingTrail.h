
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
	//if every line segment


	class ISweepingTrail:
		public CRenderSettingBlendMode,
		public CRenderSettingFillMode
	{
	public:

		//set current header line segments (in World space coordinate)
		void SetHeader(N_LineSegment lineSeg);

		//header segment
		N_LineSegment GetHeader();

		//the center of the header segment
		NVECTOR3 GetHeaderCenterPos();

		//time limit of cooling down the header line segment and GENERATE a new "free" header
		//unit: Milli-second
		void SetHeaderCoolDownTimeThreshold(float duration_ms);

		//!!!!Every line segment has its life time.
		//(which means, the last LS might not approach to second last LS at the beginning)
		//(which also means, the last line segment's texcoord.u might be much less than 1 for a while)
		//unit: Milli-second
		void SetMaxLifeTimeOfLineSegment(float duration_ms);

		//
		uint32_t GetActiveVerticesCount();

		//spline interpolation steps (count of sub-regions divided into)
		void SetInterpolationStepCount(uint32_t count);

		//updating vertices (to gpu vertex buffer):
		//1. header line segment's position 
		//2. tail line segments position
		//3. every vertices' uv (actually only texcoord.u changes)
		void Update(float deltaTime);

		bool IsRenderable();

	private:

		//curved interpolation is needed, use tangent for Cubic Hermite
		struct N_GenQuadInfo
		{
			uint32_t interpolation_steps;//final sub-regions count. e.g. 5 steps, each 0.2 lerp ratio, 4 cuts
			NVECTOR3 frontPos1;
			NVECTOR3 frontPos2;
			NVECTOR3 frontTangent1;//estimated tangent (for Cubic Hermite Spline)
			NVECTOR3 frontTangent2;//estimated tangent (for Cubic Hermite Spline)
			NVECTOR3 backPos1;
			NVECTOR3 backPos2;
			NVECTOR3 backTangent1;//estimated tangent (for Cubic Hermite Spline)
			NVECTOR3 backTangent2;//estimated tangent (for Cubic Hermite Spline)
		};

		friend class ISweepingTrailManager;
		friend IFactory<ISweepingTrail>;
		friend class IRenderModuleForSweepingTrailFX;
		typedef N_SimpleVertex N_SweepingTrailVertexType;

		ISweepingTrail();

		~ISweepingTrail();

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGpuBuffer(UINT vertexPoolSize);

		// CoolingDown
		void mFunction_CoolDownHeader();

		//Collapsing & Movement
		void mFunction_MoveAndCollapseTail();

		//Gen actually vertices to draw
		void mFunction_GenVerticesAndUpdateToGpuBuffer();

		//
		float mFunction_UtDistanceBetweenLine(N_LineSegment& line1,N_LineSegment& line2);

		//calculate given line segment's life timer
		float mFunction_UtComputeLSLifeTimer(int index);

		//generated interpolated quad group(between a pair of line segment)
		int mFunction_UtGenQuad(const N_GenQuadInfo& desc, float frontLifeTimer, float backLifeTimer, N_SweepingTrailVertexType* quad);	//return vertices generated

		//estimate tangent of given point
		void mFunction_UtEstimateTangent(int currentLineSegmentIndex, NVECTOR3& outTangent1, NVECTOR3& outTangent2);

		ID3D11Buffer*	 m_pVB_Gpu;//(2018.7.23)simple vertex
		UINT mGpuVertexPoolByteCapacity;

		//line segments list (free/dynamic HEADER line segments are EXCLUDED)
		std::vector<Noise3D::N_LineSegment> mFixedLineSegments;
		N_LineSegment mFreeHeader;//keep updating by "SetHeader" until certain 'cool down time' is reached
		N_LineSegment mFreeTail_Start;//the tail LS keep approaching to the second last LS. And the lerp start should be saved.
		N_LineSegment mFreeTail_Current;//the tail LS keep approaching to the second last LS.
		float mTailQuadCollapsingRatio;//[0,1] ratio for the line segment vertex to lerp from mFreeTail_Start to mFreeTail_Current
		int mInterpolationStepCount;//steps for cubic hermite interpolation

		float mHeaderCoolDownTimeThreshold;//after given time, the header segment will be fixed down and add to "Cooled down line segments"
		float mHeaderCoolDownTimer;	//timer (initially 0, increment)
		//float mHeaderCoolDownDistanceThreshold;//if the distance between the first & second line is larger than given distance, the header segment will be fixed down and add to "Cooled down line segments"
		float mMaxLifeTimeOfLS;//the maximum time each line segment can live

	};
}