
/***********************************************************************

			Spherical Harmonic Rotation implementation

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::GI::SHRotationWignerMatrix::SHRotationWignerMatrix(uint32_t highestBandIndex)
{
	mHighestBandIndex = highestBandIndex;
	if (highestBandIndex >= 100)
	{
		ERROR_MSG("Wigner Matrix(SH Rotation): band width is too large. Try less bands.");
		mMat.resize(0);
	}
	else
	{
		mMat.resize(mHighestBandIndex);
		for (uint32_t i = 0; i <= highestBandIndex; ++i)
		{
			mMat.at(i).resize((2 * i + 1)*(2 * i + 1), 0.0f);
		}
	}
}

float Noise3D::GI::SHRotationWignerMatrix::At(uint32_t l, uint32_t row, uint32_t col)
{
	//the l-th (l>=0) band Wigner Matrix
	return mMat.at(l).at((2*l +1) * row + col);
}

float Noise3D::GI::SHRotationWignerMatrix::At(uint32_t l, int m, int n)
{
	//in l-th band Wigner Matrix, -L<=m<=L 
	return mMat.at(l).at((2 * l + 1)*(int(l) - m) + (int(l) - n)) ;
}

void Noise3D::GI::SHRotationWignerMatrix::Set(uint32_t l, uint32_t row, uint32_t col,float val)
{
	//the l-th (l>=0) band wigner matrix
	mMat.at(l).at((2 * l + 1) * row + col) = val;
}

void Noise3D::GI::SHRotationWignerMatrix::Set(uint32_t l, int m, int n, float val)
{
	//the l-th (l>=0) band wigner matrix
	mMat.at(l).at((2 * l + 1)*(int(l) - m) + (int(l) - n)) = val;
}

void Noise3D::GI::SHRotationWignerMatrix::Rotate(RigidTransform t, std::vector<NColor4f>& in_out_SHVector)
{
	//1. ZYZ euler angle decomposition
	mFunction_RotateZ
	//2. Rotate Z, construct Y and multiply

}

/**********************************************

							PRIVATE

***********************************************/
void Noise3D::GI::SHRotationWignerMatrix::mFunction_RotateZ(float angle, std::vector<NColor4f>& in_out_SHVector)
{
	//WARNING: SHVector starts from -m to m (different convention from Wigner Matrix)
	//Reference: <Fast Approximation to Spherical Harmonic Rotation> Krivanek, Appendix B
	for (uint32_t L= 0; L <= mHighestBandIndex; ++L)
	{
		for (int M = 1; M <=  int(L); ++M)
		{
			NColor4f c_L_negM = in_out_SHVector.at(GI::SH_FlattenIndex(L, -M));//element of index -m in this band
			NColor4f c_L_posM = in_out_SHVector.at(GI::SH_FlattenIndex(L, M));//element of index m in this band
			float cos_m_alpha = cos(M * angle);
			float sin_m_alpha = sin(M * angle);
			in_out_SHVector.at(GI::SH_FlattenIndex(L, M)) = c_L_negM * cos_m_alpha - c_L_posM *sin_m_alpha;
			in_out_SHVector.at(GI::SH_FlattenIndex(L, M)) = c_L_negM * sin_m_alpha + c_L_posM * cos_m_alpha;
		}
	}
}
