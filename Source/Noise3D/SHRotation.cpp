
/***********************************************************************

			Spherical Harmonic Rotation implementation

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

Noise3D::GI::SHRotationWignerMatrix::SHRotationWignerMatrix(uint32_t highestBandIndex):
	mPreviousEulerY(0.0f)
{
	mHighestBandIndex = highestBandIndex;
	if (highestBandIndex >= 100)
	{
		ERROR_MSG("Wigner Matrix(SH Rotation): band width is too large. Try less bands.");
		mMat.resize(0);
	}
	else
	{
		mMat.resize(mHighestBandIndex+1);
		for (uint32_t i = 0; i <= highestBandIndex; ++i)
		{
			mMat.at(i).resize((2 * i + 1)*(2 * i + 1), 0.0);
		}
	}
}

float Noise3D::GI::SHRotationWignerMatrix::GetByRowCol(uint32_t l, uint32_t row, uint32_t col)
{
	//the l-th (l>=0) band Wigner Matrix
	return mMat.at(l).at((2*l +1) * row + col);
}

float Noise3D::GI::SHRotationWignerMatrix::GetByIndex(uint32_t l, int m, int n)
{
	if (m >= -int(l) && m <= int(l) && n >= -int(l) && n <= int(l))
	{
		//in l-th band Wigner Matrix, -L<=m<=L ,
		// row = l - m
		// column = l - n
		return mMat.at(l).at((2 * l + 1)*(int(l) - m) + (int(l) - n));
	}
	else
	{
		//reference of the special extension:<FFTs on the Rotation Group> Peter J Kostelec
		//the recursive construction of Wigner Matrix Rotation Y might need this term.
		return 0.0f;
	}
}

void Noise3D::GI::SHRotationWignerMatrix::SetByRowCol(uint32_t l, uint32_t row, uint32_t col,float val)
{
	//the l-th (l>=0) band wigner matrix
	mMat.at(l).at((2 * l + 1) * row + col) = val;
}

void Noise3D::GI::SHRotationWignerMatrix::SetByIndex(uint32_t l, int m, int n, float val)
{
	//the l-th (l>=0) band wigner matrix
	mMat.at(l).at((2 * l + 1)*(int(l) - m) + (int(l) - n)) = val;
}

void Noise3D::GI::SHRotationWignerMatrix::Multiply(RigidTransform t, const std::vector<Color4f>& in_SHVector, std::vector<Color4f>& out_SHVector)
{
	if ((mHighestBandIndex + 1)*(mHighestBandIndex + 1) != in_SHVector.size())
	{
		ERROR_MSG("SHRotationWignerMatrix: multiplication dimension not match!");
		return;
	}

	//intermediate sh vector
	std::vector<Color4f> tmpShVector=in_SHVector;

	//0. ZYZ euler angle decomposition
	N_EULER_ANGLE_ZYZ euler = t.GetEulerAngleZYZ();

	//1. Mul Z (without matrix construction)
	mFunction_RotateZ(euler.angleZ1, in_SHVector, tmpShVector);

	//2. Mul Y (explicitly construct Wigner Matrix)
	//reference : Lisle's <Algorithms for Spherical Harmonic Lighting>
	if (euler.angleY2 != mPreviousEulerY)
	{
		//actually construction of rotation Y matrix is not so efficient, avoid as possible as we can.
		mFunction_ConstructRotationY(euler.angleY2);
		mFunction_MultiplyRotationY(tmpShVector);
		mPreviousEulerY = euler.angleY2;

		//out_SHVector = tmpShVector;
	}

	//3. Mul Z (without matrix construction)
	mFunction_RotateZ(euler.angleZ3, tmpShVector,  out_SHVector);


}

/**********************************************

							PRIVATE

***********************************************/
void Noise3D::GI::SHRotationWignerMatrix::mFunction_RotateZ(float angle, const std::vector<Color4f>& in_ShVector, std::vector<Color4f>& out_SHVector)
{
	//WARNING: SHVector starts from -m to m (different convention from Wigner Matrix)
	//Reference: <Fast Approximation to Spherical Harmonic Rotation> Krivanek, Appendix B
	for (uint32_t L= 0; L <= mHighestBandIndex; ++L)
	{
		out_SHVector.at(GI::SH_FlattenIndex(L, 0)) = in_ShVector.at(GI::SH_FlattenIndex(L, 0));
		for (int M = 1; M <=  int(L); ++M)
		{
			Color4f c_L_negM = in_ShVector.at(GI::SH_FlattenIndex(L, -M));//element of index -m in this band
			Color4f c_L_posM = in_ShVector.at(GI::SH_FlattenIndex(L, M));//element of index m in this band
			float cos_m_alpha = cos(M * angle);
			float sin_m_alpha = sin(M * angle);
			out_SHVector.at(GI::SH_FlattenIndex(L, -M)) = c_L_negM * cos_m_alpha - c_L_posM *sin_m_alpha;
			out_SHVector.at(GI::SH_FlattenIndex(L, M)) = c_L_negM * sin_m_alpha + c_L_posM * cos_m_alpha;
		}
	}
}

void Noise3D::GI::SHRotationWignerMatrix::mFunction_ConstructRotationY(float angle)
{
	float cosBeta = cos(angle);
	float sinBeta = sin(angle);

	//constructing Wigner Matrix - Rotation Y
	//1.harcode first 2 bands
	if (mHighestBandIndex >= 0)
	{
		SHRotationWignerMatrix::SetByRowCol(0, 0, 0, 1.0f);
	}
	
	if (mHighestBandIndex >= 1)
	{
		//(2018.10.27)band 1 wigner matrix of rotation Y remains un-determined. Can't find support literature.
		//reference: 
		// ***Lisle's <Algorithms for Spherical Harmonic Lighting>£¬ but don't know the correspondence between RowCol & index
		// ***Peter J Kostelec's<FFTs on the Rotation Group>

		/*SHRotationWignerMatrix::SetByRowCol(1, 0, 0, 1.0f);
		SHRotationWignerMatrix::SetByRowCol(1, 0, 1, 0.0f);
		SHRotationWignerMatrix::SetByRowCol(1, 0, 2, 0.0f);

		SHRotationWignerMatrix::SetByRowCol(1, 1, 0, 0.0f);
		SHRotationWignerMatrix::SetByRowCol(1, 1, 1, cos(angle));
		SHRotationWignerMatrix::SetByRowCol(1, 1, 2, sin(angle));

		SHRotationWignerMatrix::SetByRowCol(1, 2, 0, 0.0f);
		SHRotationWignerMatrix::SetByRowCol(1, 2, 1, -sin(angle));
		SHRotationWignerMatrix::SetByRowCol(1, 2, 2, cos(angle));*/

		SHRotationWignerMatrix::SetByIndex(1, -1, -1, 1.0f);
		SHRotationWignerMatrix::SetByIndex(1, -1, 0, 0.0f);
		SHRotationWignerMatrix::SetByIndex(1, -1, 1, 0.0f);

		SHRotationWignerMatrix::SetByIndex(1, 0, -1, 0.0f);
		SHRotationWignerMatrix::SetByIndex(1, 0, 0, cosBeta);
		SHRotationWignerMatrix::SetByIndex(1, 0, 1, sinBeta);

		SHRotationWignerMatrix::SetByIndex(1, 1, -1, 0.0f);
		SHRotationWignerMatrix::SetByIndex(1, 1, 0, -sinBeta);
		SHRotationWignerMatrix::SetByIndex(1, 1, 1, cosBeta);
		
	}

	if(mHighestBandIndex >= 2)
	{
		for (int L = 2; L <= int(mHighestBandIndex); ++L)
		{
			//**2.corners (L,L) (-L,-L)
			float cornerVal1 = SHRotationWignerMatrix::GetByIndex(L - 1, L - 1, L - 1);
			float cornerVal2 = SHRotationWignerMatrix::GetByIndex(L - 1, -L + 1, -L + 1);
			SHRotationWignerMatrix::SetByIndex(L, L, L, 0.5f * cosBeta * cornerVal1 + 0.5f * cornerVal2);
			SHRotationWignerMatrix::SetByIndex(L, -L, -L, 0.5f * cornerVal1 + 0.5f * cosBeta * cornerVal2);

			//**3. half column (M,L) (-M,-L), and symmetric half row (L,M) (-L,-M)
			//two marginal (half) columns, and their symmetric (half) rows about diagonal
			for (int M = 0; M <= L - 1; ++M)
			{
				float scaleFactor = sqrt((float(L) * (float(L) - 0.5f)) / float(L*L - M*M)) * sinBeta;

				float prevTerm1 = SHRotationWignerMatrix::GetByIndex(L - 1, M, L - 1);
				SHRotationWignerMatrix::SetByIndex(L, M, L, scaleFactor* prevTerm1);
				float prevTerm2 = SHRotationWignerMatrix::GetByIndex(L - 1, -M, -L + 1);
				SHRotationWignerMatrix::SetByIndex(L, -M, -L, scaleFactor * prevTerm2);

				//symmetric/transpose position (additional variables for clarity)
				int symmetricM = L;
				int symmetricN = M;
				SHRotationWignerMatrix::SetByIndex(L, symmetricM, symmetricN, float(pow(-1, symmetricM - symmetricN)) * scaleFactor* prevTerm1);
				SHRotationWignerMatrix::SetByIndex(L, -symmetricM, -symmetricN, float(pow(-1, symmetricN - symmetricM)) * scaleFactor* prevTerm2);
			}
		
			//**4. interior region #1, block diagonal
			for (int M = 0; M <= L - 1; ++M)
			{
				for (int N = 0; N <= L - 1; ++N)
				{
					float factor1 = L * (2 * L - 1) / sqrt(float( L * L - M * M ) * float( L * L  - N * N ));
					float factor2 = M * N / float(L * (L - 1));
					float factor3 = sqrtf(   float(((L - 1)*(L - 1) - M*M) * ((L - 1)*(L - 1) - N*N))  ) / float((L - 1)*(2 * L - 1));

					float d_Lminus1_pos_mn = SHRotationWignerMatrix::GetByIndex(L - 1, M, N);
					float d_Lminus2_pos_mn = SHRotationWignerMatrix::GetByIndex(L - 2, M, N);//L's out-of-range situation has been considered (return 0)
					float d_Lminus1_neg_mn = SHRotationWignerMatrix::GetByIndex(L - 1, -M, -N);
					float d_Lminus2_neg_mn = SHRotationWignerMatrix::GetByIndex(L - 2, -M, -N);//L's out-of-range situation has been considered (return 0)

					//float result1 = factor1 * (cosBeta * d_Lminus1_pos_mn - factor2 * d_Lminus1_neg_mn - factor3 * d_Lminus2_pos_mn);
					//float result2 = factor1 * (cosBeta * d_Lminus1_neg_mn - factor2 * d_Lminus1_pos_mn - factor3 * d_Lminus2_pos_mn);

					//WARNING: Lisle 2007 <Algorithm for Spherical Harmonic Lighting has a very KENG DIE error
					//in the right side of the d^l_(m,n) recursive equation, d^(l-2)_(-m,-n) should be used instead of d^(l-2)_(m,n) 
					float result1 = factor1 * (cosBeta * d_Lminus1_pos_mn - factor2 * d_Lminus1_neg_mn - factor3 * d_Lminus2_pos_mn);
					float result2 = factor1 * (cosBeta * d_Lminus1_neg_mn - factor2 * d_Lminus1_pos_mn - factor3 * d_Lminus2_neg_mn);

					SHRotationWignerMatrix::SetByIndex(L, M, N, result1);
					SHRotationWignerMatrix::SetByIndex(L, -M, -N, result2);
	
				}
			}

			//**5. interior region #2, the remaining elements are filled with 0
			//(most of them are initialized to 0 and remained untouched,only the central 1 row and 1 column need to be cleared to 0)
			for (int i = 1; i <= L; ++i)
			{
				SHRotationWignerMatrix::SetByIndex(L, 0, -i, 0.0f);
				SHRotationWignerMatrix::SetByIndex(L, -i, 0, 0.0f);
			}

		}

	}

}

void Noise3D::GI::SHRotationWignerMatrix::mFunction_MultiplyRotationY(std::vector<Color4f>& in_out_SHVector)
{
	//Multiplication of Wigner Matrix and SH vector
	for (int l = 0; l <= int(mHighestBandIndex); ++l)
	{
		//for every SH vector band, assign the result of WignerMatrix * band (manipulate 4 channels simultaneously)
		for (int m = -l; m <= l; ++m)
		{
			Color4f result = Color4f(0, 0, 0, 0);
			for (int n = -l; n <= l; ++n)
			{
				result += in_out_SHVector.at(GI::SH_FlattenIndex(l, n)) * SHRotationWignerMatrix::GetByIndex(l, m, n);
			}
			in_out_SHVector.at(GI::SH_FlattenIndex(l, m)) = result;
		}
	}
}
