
/***********************************************************************

								h��SweepingTrail

	A Sweeping trail several main characteristic:
	1. a trail path is driven by a sequence of spatial line segments
	2. adjacent line segments pairs can build up 2 triangles ( or a spatial 'quad')
	3. the header of the trail (the head line segment) must be updated in every frame
	4. the header of the trail should be "cooled down" when reached a given time duration limit.
			A new "free" header line trail shall be generated by then.
	5. the tail of the trail(the last line segment) must be updated in every frame, and
			approaching to the second last line segment over time to reduce the area 
			of the last quad (Meanwhile, the u coord of the tail vertices is maintained to 1)
	6. (2018.7.23)the headers' vertices u coord should be 0, while the tail should be 1.

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::ISweepingTrail::ISweepingTrail()
{
}

Noise3D::ISweepingTrail::~ISweepingTrail()
{

}

void Noise3D::ISweepingTrail::SetHeaderLineSegment(N_LineSegment lineSeg)
{
}

void Noise3D::ISweepingTrail::SetHeaderCoolDownTime(float duration)
{
}

void Noise3D::ISweepingTrail::SetTailShrinkingSpeed(float duration)
{
}

void Noise3D::ISweepingTrail::Update(float deltaTime)
{
}
