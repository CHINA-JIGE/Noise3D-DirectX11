//uinit test for rigid transform class
#include "Noise3D.h"
#include <iostream>

using namespace Noise3D;
const int c_bandWidth = 4;
GI::SHRotationWignerMatrix wm(c_bandWidth);

void OutputWignerMatrixByIndex()
{
	for (int L = 0; L <= c_bandWidth; ++L)
	{
		std::cout << "***Matrix Band" << L << "***" << std::endl;
		for (int m = -L; m <= L; ++m)
		{
			for (int n = -L; n <= L; ++n)
			{
				std::cout << "(" << m << "," << n << "):" << wm.GetByIndex(L, m, n) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

void OutputWignerMatrixByRowCol()
{
	for (int L = 0; L <= c_bandWidth; ++L)
	{
		std::cout << "***Matrix Band" << L << "***" << std::endl;
		for (int m = 0; m < 2 * L + 1; ++m)
		{
			for (int n = 0; n < 2 * L + 1; ++n)
			{
				std::cout << wm.GetByRowCol(L, m, n) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

void UnitTest_WignerMatrixAccess()
{
	for (int L = 0; L <= c_bandWidth; ++L)
	{
		for (int row = 0; row < 2*L+1; ++row)
		{
			for (int col = 0; col < 2 * L + 1; ++col)
			{
				wm.SetByRowCol(L, row, col, row + 0.1f* col);
			}
		}
	}

	OutputWignerMatrixByRowCol();
	OutputWignerMatrixByIndex();
};

void UnitTest_WignerMatrixConstruction()
{
	RigidTransform t;
	//t.SetRotation(NVECTOR3(0.3f, 0.5f, 0.7f));
	t.SetRotation(NVECTOR3(0, 0, 0.7f));

	std::vector<NColor4f> shVector((c_bandWidth+1)*(c_bandWidth+1));
	float R[25] = { 1.11153f, -0.1041f, 0.111798f, -0.185567f, 0.228846f, -0.204815f, 0.0740972f, 0.254062f, -0.0690228f, -0.148957f, -0.0467964f, -0.00283712f, -0.0127956f, 0.0533392f, 0.0649491f, 0.0997128f, 0.0146833f, 0.149934f, 0.0832279f, 0.0812946f, 0.0678353f, -0.0678564f, -0.0755984f, -0.0230365f, -0.0361736 };
	float G[25] = { 1.75824f, -0.494459f, 0.256176f, -0.188216f, 0.19415f, -0.210385f, -0.044763f, 0.0517955f, 0.144605f, -0.083788f, -0.0273498f, -0.13f, -0.110406f, 0.0205335f, -0.0857498f, 0.0502479f, 0.0662013f, 0.0784781f, 0.0341005f, 0.0662297f, -0.047903f, -0.0351795f, -0.05699f, -0.0591753f, -0.0881703 };
	float B[25] = { 2.50516f, -0.755325f, 0.201223f, -0.00976831f, 0.0461074f, 0.0678042f, -0.144301f, -0.0724294f, 0.253208f, -0.0181658f, 0.0212355f, -0.151095f, -0.0783888f, -0.0507256f, -0.138607f, 0.0415338f, 0.0129188f, 0.00229847f, -0.00642102f, 0.0671316f, -0.0958311f, -0.0632111f, -0.0239553f, -0.0450559f, -0.122071 };
	float A[25] = { 3.54585f, -0.0339506f, 0.0156747f, 0.0208968f, 0.0262324f, -0.0378239f, 0.0240198f, -0.0517252f, 0.0336873f, 0.0297781f, -0.016357f, -0.0127828f, -0.0397653f, -0.0343735f, -0.0318757f, 0.0349987f, -0.0166717f, -0.0332837f, -0.0287968f, 0.0106524f, -0.0205633f, -0.0321655f, 3.12727e-5f, 0.0237876f, -0.0550001 };
	for (int i = 0; i < 25; ++i)
	{
		shVector.at(i) = NColor4f(R[i], G[i], B[i], A[i]);
	}

	wm.Multiply(t, shVector);
	OutputWignerMatrixByIndex();
}

int main()
{
	UnitTest_WignerMatrixConstruction();
	system("pause");
	return 0;
}