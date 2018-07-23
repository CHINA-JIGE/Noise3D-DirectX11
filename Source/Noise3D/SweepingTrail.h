
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
		void Update();

	private:

		friend ISweepingTrailManager;
		friend IFactory<ISweepingTrail>;

		ISweepingTrail();

		~ISweepingTrail();


		std::vector<Noise3D::Ut::
	};
}