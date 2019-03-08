
/***********************************************************************

				Affine Transform (based on rigid transform)

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::AffineTransform::AffineTransform():
	mScale(1.0f,1.0f,1.0f)
{
}

void Noise3D::AffineTransform::SetScale(float scaleX, float scaleY, float scaleZ)
{
	mScale = NVECTOR3(scaleX, scaleY, scaleZ);
}

void Noise3D::AffineTransform::SetScale(NVECTOR3 s)
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

NVECTOR3 Noise3D::AffineTransform::GetScale() const
{
	return mScale;
}

void Noise3D::AffineTransform::GetTransformMatrix(NMATRIX & outTransformMat) const
{
	NMATRIX	tmpMatrixScaling;
	NMATRIX	tmpMatrixTranslation;
	NMATRIX	tmpMatrixRotation;

	//1.scale
	tmpMatrixScaling = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	//2.rotate
	tmpMatrixRotation = RigidTransform::GetRotationMatrix();

	//3.translate
	tmpMatrixTranslation = XMMatrixTranslationFromVector(RigidTransform::GetPosition());

	//4.concatenate scaling/rotation/translation
	NMATRIX tmpMatrix = XMMatrixMultiply(tmpMatrixScaling, tmpMatrixRotation);
	outTransformMat = XMMatrixMultiply(tmpMatrix, tmpMatrixTranslation);
}

void Noise3D::AffineTransform::GetTransformMatrix(NMATRIX& outWorldMat, NMATRIX& outWorldInvTransposeMat) const
{
	NMATRIX	tmpMatrixScaling;
	NMATRIX	tmpMatrixTranslation;
	NMATRIX	tmpMatrixRotation;

	//1.scale
	tmpMatrixScaling = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	//2.rotate
	tmpMatrixRotation = RigidTransform::GetRotationMatrix();

	//3.translate
	tmpMatrixTranslation = XMMatrixTranslationFromVector(RigidTransform::GetPosition());

	//4.concatenate scaling/rotation/translation
	NMATRIX tmpMatrix = XMMatrixMultiply(tmpMatrixScaling, tmpMatrixRotation);
	outWorldMat = XMMatrixMultiply(tmpMatrix, tmpMatrixTranslation);

	//world inv transpose for normal's transformation
	NMATRIX worldInvMat = XMMatrixInverse(nullptr, outWorldMat);
	if (XMMatrixIsInfinite(worldInvMat))
	{
		ERROR_MSG("world matrix Inv not exist! determinant == 0 ! ");
		outWorldInvTransposeMat = XMMatrixIdentity();
		return;
	}
	else
	{
		outWorldInvTransposeMat = worldInvMat.Transpose();
	}
}

void Noise3D::AffineTransform::SetTransform(const AffineTransform & t)
{
	RigidTransform::SetRigidTransform(t);//rotation, translation
	AffineTransform::SetScale(t.GetScale());//scale
}

bool Noise3D::AffineTransform::SetAffineMatrix(NMATRIX mat)
{
	NVECTOR3 pos;
	NQUATERNION rot;
	NVECTOR3 scale;

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
		return false;
	}
}

NVECTOR3 Noise3D::AffineTransform::TransformVector_Affine(NVECTOR3 vec) const
{
	NVECTOR3 outVec = RigidTransform::TransformVector_Rigid(NVECTOR3(vec.x * mScale.x, vec.y * mScale.y, vec.z * mScale.z));

	return outVec;
}

NVECTOR4 Noise3D::AffineTransform::TransformVector_MatrixMul(NVECTOR4 lhs, const NMATRIX& rhs)
{
	//m[Row][Column], refer to SimpleMath source code for this convention
	//(2019.3.7)Noise3D uses ROW major
	/*
	
					[00	01		02		03]
	[x y z w]	[10	11		12		13]
					[20	21		22		23]
					[30	31		32		33]
	*/

	NVECTOR4 res;
	res.x = rhs.m[0][0] * lhs.x + rhs.m[1][0] * lhs.y + rhs.m[2][0] * lhs.z + rhs.m[3][0] * lhs.w;
	res.y = rhs.m[0][1] * lhs.x + rhs.m[1][1] * lhs.y + rhs.m[2][1] * lhs.z + rhs.m[3][1] * lhs.w;
	res.z = rhs.m[0][2] * lhs.x + rhs.m[1][2] * lhs.y + rhs.m[2][2] * lhs.z + rhs.m[3][2] * lhs.w;
	res.w = rhs.m[0][3] * lhs.x + rhs.m[1][3] * lhs.y + rhs.m[2][3] * lhs.z + rhs.m[3][3] * lhs.w;
	return res;
}

NVECTOR3 Noise3D::AffineTransform::TransformVector_MatrixMul(NVECTOR3 lhs, const NMATRIX & rhs)
{
	//m[Row][Column], refer to SimpleMath source code for this convention
	//(2019.3.7)Noise3D uses ROW major
	/*
	
				[00	01		02		03]
	[x y z]	[10	11		12		13]
				[20	21		22		23]
	*/
	NVECTOR3 res;
	res.x = rhs.m[0][0] * lhs.x + rhs.m[1][0] * lhs.y + rhs.m[2][0] * lhs.z + rhs.m[3][0] * 1.0f;
	res.y = rhs.m[0][1] * lhs.x + rhs.m[1][1] * lhs.y + rhs.m[2][1] * lhs.z + rhs.m[3][1] * 1.0f;
	res.z = rhs.m[0][2] * lhs.x + rhs.m[1][2] * lhs.y + rhs.m[2][2] * lhs.z + rhs.m[3][2] * 1.0f;

	return res;
}
