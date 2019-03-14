
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

		void		SetScale(float scaleX, float scaleY, float scaleZ);

		void		SetScale(NVECTOR3 s);

		void		SetScaleX(float scaleX);

		void		SetScaleY(float scaleY);

		void		SetScaleZ(float scaleZ);

		NVECTOR3 GetScale() const;

		NMATRIX	 GetTransformMatrix() =delete;//rigid transform

		//DirectXMath use row matrix
		void		GetTransformMatrix(NMATRIX& outTransformMat) const;

		void		GetTransformMatrix(NMATRIX& outTransformMat, NMATRIX& outTransformInvTransposeMat) const;

		void		SetTransform(const AffineTransform& t);

		//could fail to decompose
		bool		SetAffineMatrix(NMATRIX mat);

		//scale, rotation, translation (based on RigidTransform::TransformVector_Rigid())
		NVECTOR3 TransformVector_Affine(NVECTOR3 vec) const;

		//matrix left-mul(row major, 1x4 * 4x4) 
		static NVECTOR4 TransformVector_MatrixMul(NVECTOR4 lhs, const NMATRIX& rhs);

		//	matrix left-mul(row major, 1x3 * 3x4)
		static NVECTOR3 TransformVector_MatrixMul(NVECTOR3 lhs, const NMATRIX& rhs);

	private:

		NVECTOR3 mScale;

	};
}