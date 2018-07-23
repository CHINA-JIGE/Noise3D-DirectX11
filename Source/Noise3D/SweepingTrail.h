
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

	class ISweepingTrail
	{
	public:

		//set current header line segments
		void SetHeaderLineSegment(N_LineSegment lineSeg);

		//time limit of cooling down the header line segment and GENERATE a new "free" header
		void SetHeaderCoolDownTime(float duration);

		//determined how fast the tail approaching to the second last line segment, measured in lerp ratio
		//e.g. if duration==10.0f, then the last quad will de-generate into a line segment after 10ms, and the tail
		//segment will be DESTROYED.
		void SetTailShrinkingSpeed(float duration);

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


		//cooled down line segments list (free/dynamic header and tail line segment are not included)
		std::vector<Noise3D::N_LineSegment> mCooledDownLineSegments;
		N_LineSegment mFreeHeader;//keep updating by "SetHeader" until certain 'cool down time' is reached
		N_LineSegment mFreeTail;//keep approaching to the second last line segment
		float mHeaderCoolDownTime;//after given time, the header segment will be fixed down and add to "Cooled down line segments"
		float mTailShrinkingSpeed;//the last quad's shrinking speed/ last LS's moving speed/ last LS's u-texcoord decreasing speed

		float mHeaderCoolDownTimer;
	};
}