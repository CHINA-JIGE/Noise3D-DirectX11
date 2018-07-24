
/***********************************************************************

						h£ºSweeping Trail Manager

		Desc: Sweeping Trail is an special effect that uses
		a sequence of triangle strip to simulate/visualize a
		path that a line segment sweep through 3D spaces.
		**Some example application scenerio: a melee weapon
		swinging in the air, following by a colored 'trail'.

		picture ref:https://blog.csdn.net/cbbbc/article/details/70863580

		you can "SetHeaderLineSegment" every frame to indicate the head of trail,
		and the trail quad sequence will automatically generate as long as 
		you call "Update(dt)" every frame.
************************************************************************/

#pragma once

namespace Noise3D
{

	class ISweepingTrail:
		public CRenderSettingBlendMode,
		public CRenderSettingFillMode
	{
	public:

		//set current header line segments (in World space coordinate)
		void SetHeaderLineSegment(N_LineSegment lineSeg);

		//time limit of cooling down the header line segment and GENERATE a new "free" header
		void SetHeaderCoolDownTimeThreshold(float duration);

		//the "snake" cool down based on movement distance of  the header
		//void SetHeaderCoolDownDistance(float distance);


		//how many time it takes to collapse the last quad into one line segment (and removed)
		//e.g. if duration==10.0f, then the last quad will de-generate into a line segment after 10ms, and the tail
		//segment will be DESTROYED.
		void SetTailCollapsedTime(float duration);

		//updating vertices (to gpu vertex buffer):
		//1. header line segment's position 
		//2. tail line segments position
		//3. every vertices' uv (actually only texcoord.u changes)
		void Update(float deltaTime);

		bool IsRenderable();

	private:

		friend class ISweepingTrailManager;
		friend IFactory<ISweepingTrail>;

		ISweepingTrail();

		~ISweepingTrail();

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGpuBuffer(UINT vertexPoolSize);

		void mFunction_CoolDownHeader();// CoolingDown

		void mFunction_MoveAndCollapseTail();//Collapsing & Movement

		void mFunction_UpdateVertexBufferInMem();//Pos and UV

		void mFunction_UpdateToGpuBuffer();

		float mFunction_Util_DistanceBetweenLine(N_LineSegment& line1,N_LineSegment& line2);

		void mFunction_Util_GenQuad(N_LineSegment& front, N_LineSegment& back, N_SimpleVertex* quad);

		typedef N_SimpleVertex N_SweepingTrailVertexType;
		std::vector<N_SweepingTrailVertexType>	mVB_Mem;//actual vertices in CPU memory, updated based on line segment list
		ID3D11Buffer*	 m_pVB_Gpu;//(2018.7.23)simple vertex
		UINT mGpuVertexPoolCapacity;

		//line segments list (free/dynamic HEADER line segments are EXCLUDED)
		std::vector<Noise3D::N_LineSegment> mFixedLineSegments;
		N_LineSegment mFreeHeader;//keep updating by "SetHeader" until certain 'cool down time' is reached
		N_LineSegment mFreeTail_Start;//the tail LS keep approaching to the second last LS. And the lerp start should be saved.
		N_LineSegment mFreeTail_Current;//the tail LS keep approaching to the second last LS.
		//bool mIsHeaderActive;
		//bool mIsTailActive;


		float mHeaderCoolDownTimeThreshold;//after given time, the header segment will be fixed down and add to "Cooled down line segments"
		float mHeaderCoolDownTimer;	//timer (initially 0, increment)
		//float mHeaderCoolDownDistanceThreshold;//if the distance between the first & second line is larger than given distance, the header segment will be fixed down and add to "Cooled down line segments"
		float mTailQuadCollapseDuration;//the last quad's collapsing time (which affects last LS's u-texcoord decreasing speed)
		float mTailQuadCollapsingTimer; //timer (initially 0, increment)
		float mTailQuadCollapsingRatio;//[0,1] ratio for the line segment vertex to lerp
	};
}