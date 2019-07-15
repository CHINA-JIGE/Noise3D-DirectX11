
/***********************************************************************

                           h£ºAffineTransform(including scale)

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/ AffineTransform :
		public RigidTransform
	{
	public:

		AffineTransform();

		AffineTransform(const AffineTransform& t);

		void		SetScale(float scaleX, float scaleY, float scaleZ);

		void		SetScale(Vec3 s);

		void		SetScaleX(float scaleX);

		void		SetScaleY(float scaleY);

		void		SetScaleZ(float scaleZ);

		Vec3 GetScale() const;

		//DirectXMath use row matrix
		Matrix	 GetAffineTransformMatrix() const;//rigid transform

		void		GetAffineTransformMatrix(Matrix& outTransformMat, Matrix& outTransformInvTransposeMat) const;

		void		GetAffineTransformMatrix(Matrix& outTransformMat, Matrix& outTransformInvMat, Matrix& outTransformInvTransposeMat) const;

		void		SetTransform(const AffineTransform& t);

		//could fail to decompose
		bool		SetAffineMatrix(Matrix mat);

		//scale, rotation, translation (based on RigidTransform::TransformVector_Rigid())
		Vec3 TransformVector_Affine(Vec3 vec) const;

		//matrix left-mul(row major, 1x4 * 4x4) 
		static Vec4 TransformVector_MatrixMul(Vec4 lhs, const Matrix& rhs);

		//	matrix left-mul(row major, 1x3 * 3x4)
		static Vec3 TransformVector_MatrixMul(Vec3 lhs, const Matrix& rhs);

	private:

		Vec3 mScale;

	};
}