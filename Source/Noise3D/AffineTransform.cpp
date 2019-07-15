
/***********************************************************************

				Affine Transform (based on rigid transform)

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::AffineTransform::AffineTransform():
	mScale(1.0f,1.0f,1.0f)
{
}

Noise3D::AffineTransform::AffineTransform(const AffineTransform & t):
	RigidTransform(t),
	mScale(t.mScale)
{
}

void Noise3D::AffineTransform::SetScale(float scaleX, float scaleY, float scaleZ)
{
	mScale = Vec3(scaleX, scaleY, scaleZ);
}

void Noise3D::AffineTransform::SetScale(Vec3 s)
{
	mScale = s;
}

void Noise3D::AffineTransform::SetScaleX(float scaleX)
{
	mScale.x = scaleX;
}

void Noise3D::AffineTransform::SetScaleY(float scaleY)
{
	mScale.y = scaleY;
}

void Noise3D::AffineTransform::SetScaleZ(float scaleZ)
{
	mScale.z = scaleZ;
}

Vec3 Noise3D::AffineTransform::GetScale() const
{
	return mScale;
}

Matrix Noise3D::AffineTransform::GetAffineTransformMatrix() const
{
	Matrix	tmpMatrixScaling;
	Matrix	tmpMatrixTranslation;
	Matrix	tmpMatrixRotation;

	//1.scale
	tmpMatrixScaling = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	//2.rotate
	tmpMatrixRotation = RigidTransform::GetRotationMatrix();

	//3.translate
	tmpMatrixTranslation = XMMatrixTranslationFromVector(RigidTransform::GetPosition());

	//4.concatenate scaling/rotation/translation
	Matrix tmpMatrix = XMMatrixMultiply(tmpMatrixScaling, tmpMatrixRotation);
	Matrix outTransformMat = XMMatrixMultiply(tmpMatrix, tmpMatrixTranslation);
	return outTransformMat;
}

void Noise3D::AffineTransform::GetAffineTransformMatrix(Matrix& outTransformMat, Matrix& outTransformInvTransposeMat) const
{
	outTransformMat = AffineTransform::GetAffineTransformMatrix();

	//world inv transpose for normal's transformation
	Matrix worldInvMat = XMMatrixInverse(nullptr, outTransformMat);
	if (XMMatrixIsInfinite(worldInvMat))
	{
		ERROR_MSG("world matrix Inv not exist! determinant == 0 ! ");
		outTransformInvTransposeMat = XMMatrixIdentity();
		return;
	}
	else
	{
		outTransformInvTransposeMat = worldInvMat.Transpose();
	}
}

void Noise3D::AffineTransform::GetAffineTransformMatrix(Matrix & outTransformMat, Matrix & outTransformInvMat, Matrix & outTransformInvTransposeMat) const
{
	outTransformMat = AffineTransform::GetAffineTransformMatrix();

	//world inv transpose for normal's transformation
	outTransformInvMat = XMMatrixInverse(nullptr, outTransformMat);
	if (XMMatrixIsInfinite(outTransformInvMat))
	{
		ERROR_MSG("world matrix Inv not exist! determinant == 0 ! ");
		outTransformInvTransposeMat = XMMatrixIdentity();
		return;
	}
	else
	{
		outTransformInvTransposeMat = outTransformInvMat.Transpose();
	}
}

void Noise3D::AffineTransform::SetTransform(const AffineTransform & t)
{
	RigidTransform::SetRigidTransform(t);//rotation, translation
	AffineTransform::SetScale(t.GetScale());//scale
}

bool Noise3D::AffineTransform::SetAffineMatrix(Matrix mat)
{
	Vec3 pos;
	Quaternion rot;
	Vec3 scale;

	//try to decompose matrix into scale/rotation/pos via SimpleMath API
	//(i haven't dig into it yet, at least don't know how to extract SCALE yet)
	bool succeeded = mat.Decompose(scale, rot, pos);
	
	if (succeeded)
	{
		RigidTransform::SetRotation(rot);
		RigidTransform::SetPosition(pos);
		AffineTransform::SetScale(scale);
		return true;
	}
	else
	{
		mat = XMMatrixIdentity();
		return false;
	}
}

Vec3 Noise3D::AffineTransform::TransformVector_Affine(Vec3 vec) const
{
	Vec3 outVec = RigidTransform::TransformVector_Rigid(Vec3(vec.x * mScale.x, vec.y * mScale.y, vec.z * mScale.z));

	return outVec;
}

Vec4 Noise3D::AffineTransform::TransformVector_MatrixMul(Vec4 lhs, const Matrix& rhs)
{
	//m[Row][Column], refer to SimpleMath source code for this convention
	//(2019.3.7)Noise3D uses ROW major
	/*
	
					[00	01		02		03]
	[x y z w]	[10	11		12		13]
					[20	21		22		23]
					[30	31		32		33]
	*/

	Vec4 res;
	res.x = rhs.m[0][0] * lhs.x + rhs.m[1][0] * lhs.y + rhs.m[2][0] * lhs.z + rhs.m[3][0] * lhs.w;
	res.y = rhs.m[0][1] * lhs.x + rhs.m[1][1] * lhs.y + rhs.m[2][1] * lhs.z + rhs.m[3][1] * lhs.w;
	res.z = rhs.m[0][2] * lhs.x + rhs.m[1][2] * lhs.y + rhs.m[2][2] * lhs.z + rhs.m[3][2] * lhs.w;
	res.w = rhs.m[0][3] * lhs.x + rhs.m[1][3] * lhs.y + rhs.m[2][3] * lhs.z + rhs.m[3][3] * lhs.w;
	return res;
}

Vec3 Noise3D::AffineTransform::TransformVector_MatrixMul(Vec3 lhs, const Matrix & rhs)
{
	//m[Row][Column], refer to SimpleMath source code for this convention
	//(2019.3.7)Noise3D uses ROW major
	/*
	
				[00	01		02		03]
	[x y z]	[10	11		12		13]
				[20	21		22		23]
	*/
	Vec3 res;
	res.x = rhs.m[0][0] * lhs.x + rhs.m[1][0] * lhs.y + rhs.m[2][0] * lhs.z + rhs.m[3][0] * 1.0f;
	res.y = rhs.m[0][1] * lhs.x + rhs.m[1][1] * lhs.y + rhs.m[2][1] * lhs.z + rhs.m[3][1] * 1.0f;
	res.z = rhs.m[0][2] * lhs.x + rhs.m[1][2] * lhs.y + rhs.m[2][2] * lhs.z + rhs.m[3][2] * 1.0f;

	return res;
}
