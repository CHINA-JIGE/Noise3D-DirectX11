/***********************************************************************

								Random Sample Generator

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

std::default_random_engine Noise3D::GI::RandomSampleGenerator::mRandomEngine = std::default_random_engine();

std::uniform_real_distribution<float> Noise3D::GI::RandomSampleGenerator::mCanonicalDist = std::uniform_real_distribution<float>(0.0, 1.0f);

std::uniform_real_distribution<float> Noise3D::GI::RandomSampleGenerator::mNormalizedDist = std::uniform_real_distribution<float>(-1.0f, 1.0f);

Noise3D::GI::RandomSampleGenerator::RandomSampleGenerator()
{
}

float Noise3D::GI::RandomSampleGenerator::CanonicalReal()
{
	return mCanonicalDist(mRandomEngine);
}

float Noise3D::GI::RandomSampleGenerator::NormalizedReal()
{
	return mNormalizedDist(mRandomEngine);
}

Vec3 Noise3D::GI::RandomSampleGenerator::UniformSphericalVec()
{
	float var1 = mCanonicalDist(mRandomEngine);
	float var2 = mCanonicalDist(mRandomEngine);

	//according to <gritty detail> and my own paper note
	//we can generate random points that uniformly distribute on a sphere 
	//by mapping a pair of canonical variable (x,y) to azimuthal angle (theta, phi) using formula:
	//(theta,phi) <-- (2arccos(sqrt(1-x)) , 2\pi * y)
	//or
	//(theta,phi)<-- (arccos(1-2x),2\pi*y)
	//but obviously, arccos(1-2x) is a alittle more computationally efficient
	float theta = acosf(1 - 2.0f * var1);
	float phi = 2 * Ut::PI * var2;

	//NOTE: this parameterization is different from the common one
	//the 'theta' start from the top of the Y-axis
	Vec3 out;
	out.x = sinf(theta)*cosf(phi);
	out.y = cos(theta);
	out.z = sinf(theta)*sinf(phi);
	return out;
}

Vec2 Noise3D::GI::RandomSampleGenerator::UniformSphericalAzimuthal()
{
	float var1 = mCanonicalDist(mRandomEngine);
	float var2 = mCanonicalDist(mRandomEngine);

	//according to <gritty detail> and my own paper note
	//we can generate random points that uniformly distribute on a sphere 
	//by mapping a pair of canonical variable (x,y) to azimuthal angle (theta, phi) using formula:
	//(theta,phi) <-- (2arccos(sqrt(1-x)) , 2\pi * y)
	//or
	//(theta,phi)<-- (arccos(1-2x),2\pi*y)
	//but obviously, arccos(1-2x) is a alittle more computationally efficient
	float theta = acosf(1 - 2.0f * var1);
	float phi = 2 * Ut::PI * var2;
	return Vec2(theta, phi);
}

Vec3 Noise3D::GI::RandomSampleGenerator::UniformSphericalVec_Cone(Vec3 normal, float maxAngle)
{
	//(2019.4.12)perahps there is no need to clamp to [0, pi]

	//(2019.4.12) similar to the original UniformSphericalVec()
	float var1 = mCanonicalDist(mRandomEngine) * maxAngle / Ut::PI;
	float var2 = mCanonicalDist(mRandomEngine);

	float theta = acosf(1 - 2.0f * var1);
	float phi = 2 * Ut::PI * var2;

	//NOTE: this parameterization is different from the common one
	//the 'theta' start from the top of the Y-axis
	//(2019.4.12)and they lie within a cone with angle 'maxAngle'
	Vec3 vec;
	vec.x = sinf(theta)*cosf(phi);
	vec.y = cosf(theta);
	vec.z = sinf(theta)*sinf(phi);

	//no need to transform
	if (normal.x==0.0f && normal.z ==0.0f && normal.y!=0.0f)return vec;

	//transform local sample to world space
	//original samples are centered vec is Y axis (0,1,0) if theta = 0;
	//now need to transform to new basis whose new Y axis is 'normal'
	normal.Normalize();
	Vec3 y_axis = Vec3(0, 1.0f, 0);

	//new basis
	Vec3 new_x_axis = y_axis.Cross(normal);
	Vec3 new_y_axis = normal;
	Vec3 new_z_axis = new_x_axis.Cross(new_y_axis);
	new_x_axis.Normalize();
	new_y_axis.Normalize();
	new_z_axis.Normalize();

	//construct transform matrix
	/*
	[ (Y x N)   (N)   ((YxN)xN)] * vec
	*/
	Vec3 matRow1 = { new_x_axis.x, new_y_axis.x ,new_z_axis.x };
	Vec3 matRow2 = { new_x_axis.y, new_y_axis.y ,new_z_axis.y };
	Vec3 matRow3 = { new_x_axis.z, new_y_axis.z ,new_z_axis.z };

	//flattened matrix mul/basis transformation
	Vec3 out;
	out.x = matRow1.Dot(vec);
	out.y = matRow2.Dot(vec);
	out.z = matRow3.Dot(vec);

	//(tried quaternion rot. 
	//but it has overhead of more arccos() and cos() sin() op)
	/*Vec3 y_axis = Vec3(0, 1.0f, 0);
	Vec3 rotAxis = y_axis.Cross(normal);
	float rotAngle = acosf(y_axis.Dot(normal));
	Quaternion q = XMQuaternionRotationAxis(rotAxis, rotAngle);*/

	return out;
}

Vec3 Noise3D::GI::RandomSampleGenerator::UniformSphericalVec_Hemisphere(Vec3 normal)
{
	return GI::RandomSampleGenerator::UniformSphericalVec_Cone(normal, Ut::PI / 2.0f);
}
