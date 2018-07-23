
/***********************************************************************

						h£ºSweeping Trail Manager

		Desc: Sweeping Trail is an special effect that uses
		a sequence of triangle strip to simulate/visualize a
		path that a line segment sweep through 3D spaces.
		**Some example application scenerio: a melee weapon
		swinging in the air, following by a colored 'trail'.

		picture ref:https://blog.csdn.net/cbbbc/article/details/70863580
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
		void SetHeaderCoolDownTime(float duration);

		//how many time it takes to collapse the last quad into one line segment (and removed)
		//e.g. if duration==10.0f, then the last quad will de-generate into a line segment after 10ms, and the tail
		//segment will be DESTROYED.
		void SetTailCollapsedTime(float duration);

		//updating vertices (to gpu vertex buffer):
		//1. header line segment's position 
		//2. tail line segments position
		//3. every vertices' uv (actually only texcoord.u changes)
		void Update(float deltaTime);

	private:

		friend class ISweepingTrailManager;
		friend IFactory<ISweepingTrail>;

		ISweepingTrail();

		~ISweepingTrail();

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_InitGpuBuffer(UINT vertexPoolSize);

		void mFunction_UpdateHeaderPos();

		void mFunction_UpdateTailPos();

		void mFunction_UpdateUV();


		typedef N_SimpleVertex N_SweepingTrailVertexType;
		std::vector<N_SweepingTrailVertexType>	mVB_Mem;//vertex in CPU memory
		ID3D11Buffer*	 m_pVB_Gpu;//(2018.7.23)simple vertex

		//line segments list (free/dynamic header and tail line segments are INCLUDED)
		//1.keep updating by "SetHeader" until certain 'cool down time' is reached
		//2.keep approaching to the second last line segment
		std::vector<Noise3D::N_LineSegment> mLineSegments;
		//N_LineSegment mFreeHeader
		//N_LineSegment mFreeTail;
		float mHeaderCoolDownTimeThreshold;//after given time, the header segment will be fixed down and add to "Cooled down line segments"
		float mTailQuadCollapseDuration;//the last quad's collapsing time (which affects last LS's u-texcoord decreasing speed)

		float mHeaderCoolDownTimer;
		float mTailQuadCollapsingTimer;
	};
}