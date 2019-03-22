
/*******************************************************

						cpp £ºLogical Sphere

********************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

Noise3D::LogicalSphere::LogicalSphere()
{
}

Noise3D::LogicalSphere::~LogicalSphere()
{
}

void Noise3D::LogicalSphere::SetRadius(float r)
{
	mRadius = r > 0.0f ? r : 1.0f;
}

float Noise3D::LogicalSphere::GetRadius()
{
	return mRadius;
}


