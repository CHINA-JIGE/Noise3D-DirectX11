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
	std::cout << "Euler Angle:" <<v.x <<" "<<v.y<<" " <<v.z<< std::endl<<std::endl;
}

int main()
{
	RigidTransform t;
	NMATRIX m;
	NMATRIX matXM;
	NVECTOR3 euler;

	//-----delta rotate with euler angle-------
	t.SetPosition(NVECTOR3(1.0f, 2.0f, 3.0f));
	t.Rotate(1.0f, 0, 0);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	matXM= XMMatrixRotationRollPitchYaw(1.0f, 0, 0);
	OutputMatrix(matXM);
	std::cout << "------------------------------" << std::endl;

	//-----delta rotate with euler angle-------
	t.SetPosition(NVECTOR3(1.0f, 2.0f, 3.0f));
	t.Rotate(0.5f,1.0f,0);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	matXM = XMMatrixRotationRollPitchYaw(1.5f, 1.0f, 0.0f);
	OutputMatrix(matXM);
	std::cout << "------------------------------" << std::endl;


	//--------delta rotate with matrix-------------
	t.SetPosition(NVECTOR3(4.0f, 5.0f, 6.0f));
	NMATRIX tmpDeltaRotMat = XMMatrixRotationAxis(NVECTOR3(1.0f, 1.0f, 1.0f), 2.0f);
	bool b=t.Rotate(tmpDeltaRotMat);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	matXM = XMMatrixRotationRollPitchYaw(1.5f, 1.0f, 0.0f);
	OutputMatrix(XMMatrixMultiply(tmpDeltaRotMat, matXM));
	std::cout << "------------------------------" << std::endl;

	//--------delta rotate with quaternion-------------
	t.Move(NVECTOR3(1.0f, 1.0f, 1.0f));
	NQUATERNION deltaRotQ = XMQuaternionRotationRollPitchYaw(1.5f, 2.0f, -1.0f);
	t.Rotate(deltaRotQ);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	matXM = XMMatrixRotationRollPitchYaw(1.5f, 1.0f, 0.0f);
	tmpDeltaRotMat = XMMatrixRotationAxis(NVECTOR3(1.0f, 1.0f, 1.0f), 2.0f);
	NMATRIX tmpDeltaRotMat2 = XMMatrixRotationRollPitchYaw(1.5f, 2.0f, -1.0f);
	t.SetRotation(tmpDeltaRotMat2);
	euler = t.GetEulerAngle();
	OutputMatrix(XMMatrixMultiply(tmpDeltaRotMat2, XMMatrixMultiply(tmpDeltaRotMat, matXM)));
	OutputEuler(euler);
	std::cout << "------------------------------" << std::endl;

	//--------delta rotate with euler angle again-------------
	NVECTOR3 oldEuler = t.GetEulerAngle();
	t.Move(NVECTOR3(1.0f, 1.0f, 1.0f));
	t.Rotate(1.0f, 1.5f, -0.35f);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(t.GetEulerAngle());

	NMATRIX tmpMat3 =XMMatrixRotationRollPitchYaw(oldEuler.x+1.0f, oldEuler.y+1.5f, oldEuler.z-0.35f);
	OutputMatrix(tmpMat3);
	std::cout << "------------------------------" << std::endl;


	//--------delta rotate with AxisAngle-------------
	oldEuler = t.GetEulerAngle();
	t.Move(NVECTOR3(-1.0f, -1.0f, -1.0f));
	t.Rotate(NVECTOR3(0.5f,1.0f,1.5f),0.5f);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(t.GetEulerAngle());

	NMATRIX tmpMat4 = XMMatrixMultiply(XMMatrixRotationAxis(NVECTOR3(0.5f, 1.0f, 1.5f), 0.5f),tmpMat3);
	OutputMatrix(tmpMat4);
	std::cout << "------------------------------" << std::endl;

	//----------absolute rotation : Quaternion---
	oldEuler = t.GetEulerAngle();
	deltaRotQ = XMQuaternionRotationAxis(NVECTOR3(1.0f, 2.0f, 3.0f), 2.0f);
	t.SetPosition(NVECTOR3(-1.0f, -1.0f, -1.0f));
	t.SetRotation(deltaRotQ);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(t.GetEulerAngle());

	matXM = XMMatrixRotationAxis(NVECTOR3(1.0f, 2.0f, 3.0f), 2.0f);
	OutputMatrix(matXM);
	std::cout << "------------------------------" << std::endl;

	//----------absolute rotation : Euler Angle---
	oldEuler = t.GetEulerAngle();
	t.SetPosition(NVECTOR3(-1.0f, -1.0f, -1.0f));
	t.SetRotation(0.123f,0.456f,0.789f);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(t.GetEulerAngle());

	matXM = XMMatrixRotationRollPitchYaw(0.123f, 0.456f, 0.789f);
	OutputMatrix(matXM);
	std::cout << "------------------------------" << std::endl;

	//----------absolute rotation : matrix,  Gimbal lock---
	oldEuler = NVECTOR3(Ut::PI / 2.0f, 0.123f, 0.789f);
	t.SetPosition(NVECTOR3(-1.0f, 5.0f , -1.0f));
	matXM = XMMatrixRotationRollPitchYaw(Ut::PI / 2.0f, 0.123f, 0.789f);
	b = t.SetRotation(matXM);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	matXM = XMMatrixRotationRollPitchYaw(oldEuler.x, oldEuler.y, oldEuler.z);
	OutputMatrix(matXM);
	OutputEuler(oldEuler);
	std::cout << "------------------------------" << std::endl;

	//----------absolute rotation : quaternion,  Gimbal lock --------
	oldEuler = NVECTOR3(-Ut::PI / 2.0f, 0.123f, 0.789f);
	t.SetPosition(NVECTOR3(-1.0f, 5.0f, -1.0f));
	deltaRotQ = XMQuaternionRotationRollPitchYaw(-Ut::PI / 2.0f, 0.123f, 0.789f);
	t.SetRotation(deltaRotQ);
	m = t.GetTransformMatrix();
	euler = t.GetEulerAngle();
	OutputMatrix(m);
	OutputEuler(euler);

	matXM = XMMatrixRotationRollPitchYaw(oldEuler.x, oldEuler.y, oldEuler.z);
	OutputMatrix(matXM);
	OutputEuler(oldEuler);
	std::cout << "------------------------------" << std::endl;

	//----------QuaternionToMatrix formula (correct)(remember to transpose)--------
	t.SetPosition(NVECTOR3(-1.0f, 5.0f, -1.0f));
	NQUATERNION q = XMQuaternionRotationAxis(NVECTOR3(1.0f, 2.0f, 3.0f), 4.0f);
	float x = q.x, y = q.y, z = q.z, w = q.w;
	matXM = XMMatrixRotationQuaternion(q);
	OutputMatrix(matXM);

	NMATRIX myMat;
	myMat.m[0][0] = 1.0f - 2.0f*y*y - 2.0f * z*z;
	myMat.m[0][1] = 2.0f * x*y - 2.0f * z*w;
	myMat.m[0][2] = 2.0f * x*z + 2.0f * y*w;
	myMat.m[1][0] = 2.0f * x*y + 2.0f * z*w;
	myMat.m[1][1] = 1.0f - 2.0f * x*x - 2.0f * z*z;
	myMat.m[1][2] = 2.0f * y*z - 2.0f * x * w;
	myMat.m[2][0] = 2.0f * x * z - 2.0f * y* w ;
	myMat.m[2][1] = 2.0f *y * z + 2.0f * x * w;
	myMat.m[2][2] = 1.0f - 2.0f*x*x - 2.0f * y *y;
	OutputMatrix(myMat);
	std::cout << "------------------------------" << std::endl;


	system("pause");
	return 0;
}