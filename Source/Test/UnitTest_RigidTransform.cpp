//uinit test for rigid transform class
#include "Noise3D.h"
#include <iostream>

using namespace Noise3D;

void OutputMatrix(const NMATRIX& mat)
{
	std::cout << "Rotation Matrix:" << std::endl;
	for (int i = 0; i< 4; ++i)
	{
		for (int j= 0; j < 4; ++j)
		{
			std::cout << mat.m[i][j] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void OutputEuler(NVECTOR3 v)
{
	std::cout << "Euler Angle:" <<v.x <<v.y <<v.z<< std::endl;
}

int main()
{
	RigidTransform t;
	NMATRIX m;
	NVECTOR3 euler;

	t.SetPosition(NVECTOR3(1.0f, 2.0f, 3.0f));
	t.Rotate(1.0f, 0, 0);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	t.SetPosition(NVECTOR3(1.0f, 2.0f, 3.0f));
	t.Rotate(0.5f,1.0f,0);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	system("pause");
	return 0;
}