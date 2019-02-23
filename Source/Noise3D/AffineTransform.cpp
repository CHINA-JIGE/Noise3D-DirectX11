
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

void Noise3D::AffineTransform::GetWorldMatrix(NMATRIX& outWorldMat, NMATRIX& outWorldInvTransposeMat) const
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

	//4.concatenate scaling/rotation/translation（跟viewMatrix有点区别）
	NMATRIX tmpMatrix = XMMatrixMultiply(tmpMatrixScaling, tmpMatrixRotation);
	outWorldMat = XMMatrixMultiply(tmpMatrix, tmpMatrixTranslation);

	//world inv transpose for normal's transformation
	NMATRIX worldInvMat = XMMatrixInverse(nullptr, outWorldMat);
	if (XMMatrixIsInfinite(worldInvMat))ERROR_MSG("Mesh: World Inv not exist! determinant == 0 ! ");
	outWorldInvTransposeMat =  worldInvMat.Transpose();
}

void Noise3D::AffineTransform::SetTransform(const AffineTransform & t)
{
	RigidTransform::SetTransform(t);//rotation, translation
	AffineTransform::SetScale(t.GetScale());//scale
}

void Noise3D::AffineTransform::SetRigidTransform(const RigidTransform & t)
{
	RigidTransform::SetTransform(t);
}

NVECTOR3 Noise3D::AffineTransform::TransformVector_Affine(NVECTOR3 vec)
{
	NVECTOR3 outVec = RigidTransform::TransformVector_Rigid(NVECTOR3(vec.x * mScale.x, vec.y * mScale.y, vec.z * mScale.z));

	return outVec;
}
