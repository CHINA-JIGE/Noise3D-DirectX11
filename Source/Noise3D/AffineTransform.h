
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

		void		GetWorldMatrix(NMATRIX& outWorldMat, NMATRIX& outWorldInvTransposeMat) const;

		void		SetTransform(const AffineTransform& t);

		void		SetRigidTransform(const RigidTransform& t);

		//scale, rotation, translation (based on RigidTransform::TransformVector_Rigid())
		NVECTOR3 TransformVector_Affine(NVECTOR3 vec) const;

	private:

		NVECTOR3 mScale;

	};
}