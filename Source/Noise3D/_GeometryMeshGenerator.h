/***********************************************************************

					h : Geometry Generator (basic logic/topology)

************************************************************************/

#pragma once

namespace Noise3D
{
	class /*_declspec(dllexport)*/  IGeometryMeshGenerator
	{
	public:

		void	CreatePlane(float fWidth, float fHeight, UINT iRowCount, UINT iColumnCount, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void CreateBox(float fWidth, float fHeight, float fDepth, UINT iDepthStep, UINT iWidthStep, UINT iHeightStep, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void	CreateSphere(float fRadius, UINT iColumnCount, UINT iRingCount, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void CreateCylinder(float fRadius, float fHeight, UINT iColumnCount, UINT iRingCount, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void	CreateSkyDome(float fRadiusXZ, float fHeight, UINT iColumnCount, UINT iRingCount, std::vector<N_SimpleVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void	CreateSkyBox(float fWidth, float fHeight, float fDepth, std::vector<N_SimpleVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

	private:

		//help creating a box or plane
		void		mFunction_Build_A_Quad(NVECTOR3 vOriginPoint, NVECTOR3 vBasisVector1, NVECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex, std::vector<N_DefaultVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

		void		mFunction_Build_A_Quad(NVECTOR3 vOriginPoint, NVECTOR3 vBasisVector1, NVECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex, std::vector<N_SimpleVertex>& outVerticeList, std::vector<UINT>& outIndicesList);

	};
}