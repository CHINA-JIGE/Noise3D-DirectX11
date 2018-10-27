//uinit test for rigid transform class
#include "Noise3D.h"
#include <iostream>

using namespace Noise3D;
GI::SHRotationWignerMatrix wm(4);


void UnitTest_WignerMatrixAccess()
{
	for (int L = 0; L <= 4; ++L)
	{
		for (int row = 0; row < 2*L+1; ++row)
		{
			for (int col = 0; col < 2 * L + 1; ++col)
			{
				wm.SetByRowCol(L, row, col, row + 0.1f* col);
			}
		}
	}

	for (int L = 0; L <= 4; ++L)
	{
		std::cout << "***Matrix Band"<<L<<"***"<<std::endl;
		for (int m = 0; m < 2 * L + 1; ++m)
		{
			for (int n = 0; n < 2 * L + 1; ++n)
			{
				std::cout<<wm.GetByRowCol(L, m, n) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	for (int L = 0; L <= 4; ++L)
	{
		std::cout << "***Matrix Band" << L << "***" << std::endl;
		for (int m = -L; m <= L; ++m)
		{
			for (int n = -L; n <=  L; ++n)
			{
				std::cout << "("<<m<<","<<n<<"):"<<wm.GetByIndex(L, m, n) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};

int main()
{
	UnitTest_WignerMatrixAccess();
	system("pause");
	return 0;
}