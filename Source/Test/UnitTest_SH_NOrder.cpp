//uinit test for rigid transform class
#include "Noise3D.h"
#include <iostream>

using namespace Noise3D;

void UnitTest_SH_Recursive()
{
	const int arrSize = 10;
	NVECTOR3 dirArr[arrSize] = {
		{0.5f, 0.3f, 0.5f},
		{1.0f, 0,0},
		{0, 1.0f, 0},
		{0, 0, 1.0f},
		{3.0f, -1.0f, 4.0f},
		{-6.0f,-2.4f, 3.0f},
		{7.0f,-1.0f,-2.0f},
		{1.0f, 2.0f , 3.0f},
		{-1.0f, -1.0f, -1.0f},
		{0.4f, 1.0f, -8.0f}
	};

	for (int l = 0; l <= 5; ++l)
	{
		for (int m = -l; m <= l; ++m)
		{
			std::cout << "band " << l << ", index" << m << std::endl;
			for (int i = 0; i < arrSize; ++i)
			{
				NVECTOR3 v = dirArr[i];
				std::cout << "Dir:" << v.x << " ," << v.y << " ," << v.z <<" :";
				try
				{
					std::cout << "[SH]:" << GI::SH(l, m, dirArr[i]) <<"  [K]"<<GI::SH_NormalizationTermK(l,m)<< "   [SH_Recursive]:" << GI::SH_Recursive(l, m, dirArr[i]) << std::endl;
				}
				catch(std::exception e)
				{
					std::cout << e.what();
				}
			}
			std::cout << std::endl;
		}
	}
}

void UnitTest_ALP()
{
	//GI::RandomSampleGenerator g;
	for (int l = 0; l <= 4; ++l)
	{
		for (int m = 0; m <= l; ++m)
		{
			std::cout << "band " << l << ", index" << m << std::endl;
			for (int i = 0; i < 10; ++i)
			{
				float value = (float(rand() % 10000) - 10000.0f / 2.0f) / 10000.0f;
				std::cout << "variable:" << value;
				try
				{
					std::cout << "[ALP_LowOrder]:" << GI::AssociatedLegendrePolynomial_LowOrder(l,m,value) 
						<< "   [ALP_Recursive]:" << GI::AssociatedLegendrePolynomial_Recursive(l, m, value) << std::endl;
				}
				catch (std::exception e)
				{
					std::cout << e.what();
				}
			}
			std::cout << std::endl;
		}
	}
}

void UnitTest_Factorial()
{
	for (int i = 0; i < 20; ++i)
	{
		std::cout << Ut::Factorial64(i) << " [32]"<<float(Ut::Factorial32(i))<<"  [64]"<<float(Ut::Factorial64(i))<<std::endl;
	}
	std::cout << std::endl;
}

int main()
{
	UnitTest_SH_Recursive();
	system("pause");
	return 0;
}