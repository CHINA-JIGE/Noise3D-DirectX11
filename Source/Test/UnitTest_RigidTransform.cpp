//uinit test for rigid transform class
#include "Noise3D.h"
#include <iostream>

using namespace Noise3D;

void OutputMatrix(const NMATRIX& mat)
{
	for (int i = 0; i< 4; ++i)
	{
		for (int j= 0; j < 4; ++j)
		{
			std::cout << mat.m[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

int main()
{
	RigidTransform t;
	NMATRIX m;
	t.SetPosition(NVECTOR3(1.0f, 2.0f, 3.0f));
	t.SetEulerX_Pitch(45.0f);
	m = t.GetTransformMatrix();
	OutputMatrix(m);

	t.SetPosition(NVECTOR3(1.0f, 2.0f, 3.0f));
	t.SetEulerY_Yaw(45.0f);
	m = t.GetTransformMatrix();
	OutputMatrix(m);

	system("pause");
	return 0;
}