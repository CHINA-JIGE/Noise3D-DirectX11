
/***********************************************************************

								Random Sample Generator

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class RandomSampleGenerator
		{
		public:

			RandomSampleGenerator();

			//generate canonical real number(uniformly distribute in [0,1])
			float CanonicalReal();

			//generate canonical real number(uniformly distribute in [-1,1])
			float NormalizedReal();

			//generate uniform random unit vector which distributes on unit sphere
			Vec3 UniformSphericalVec();

			//generate uniform random unit vector's( which distributes on unit sphere)azimulthal (vec.x==theta==pitch, vec.y==phi==yaw)
			Vec2 UniformSphericalAzimuthal();

			//generate uniform random unit vector distribute in a cone, given cone's generatrix and angle
			//the procedure is similar to gen uniform sample on unit sphere,
			//but a post-transform is needed.
			// maxAngle== PI stands for a whole sphere
			// maxAngle==PI/2 stands for a hemisphere
			Vec3 UniformSphericalVec_Cone(Vec3 normal, float maxAngle);

			void UniformSphericalVec_Cone(Vec3 normal, float maxAngle, int sampleCount, std::vector<Vec3>& outVecList,float& outPdf);

			Vec3 UniformSphericalVec_Hemisphere(Vec3 normal);

			void UniformSphericalVec_Hemisphere(Vec3 normal, int sampleCount, std::vector<Vec3>& outVecList);

			void UniformSphericalVec_ShadowRays(Vec3 pos, ISceneObject* pObj, int sampleCount, std::vector<Vec3>& outVecList, float& outPdf);

			//with cosine weight pdf, most density near the 'normal' direction
			void CosinePdfSphericalVec_Cone(Vec3 normal, float maxAngle, int sampleCount, std::vector<Vec3>& outVecList, std::vector<float>& outPdfList);

			void CosinePdfSphericalVec_ShadowRays(Vec3 pos, ISceneObject* pObj, int sampleCount, std::vector<Vec3>& outVecList, std::vector<float>& outPdfList);

			//shadow rays to sample a rect area light src(normal r used to calculate pdf)
			void RectShadowRays(Vec3 pos, LogicalRect* pRect, int sampleCount, std::vector<Vec3>& outVecList, std::vector<float>& outPdfList);

			//GGX NDF importance sampling (n=normal, v=view, l_center=center light vec of generated vec (the reflected dir under perfect mirror reflection))
			void GGXImportanceSampling_SpecularReflection(Vec3 l_perfectReflected, Vec3 v, Vec3 n, float ggx_alpha, int sampleCount, std::vector<Vec3>& outVecList, std::vector<float>& outPdfList);

			//GGX NDF importance sampling(for refraction) (eta: refractive index)
			void GGXImportanceSampling_SpecularTransmission(Vec3 inPath, Vec3 outPathPerfectRefracted, float eta_in, float eta_out,  Vec3 n, float ggx_alpha, int sampleCount, std::vector<Vec3>& outVecList, std::vector<float>& outPdfList);

		private:

			static std::default_random_engine mRandomEngine;

			static std::uniform_real_distribution<float> mCanonicalDist;

			static std::uniform_real_distribution<float> mNormalizedDist;

			Vec3 mFunc_UniformSphericalVecGen_AzimuthalToDir(float theta, float phi);

			void mFunc_ConstructTransformMatrixFromYtoNormal(Vec3 n, Vec3& outMatRow1, Vec3& outMatRow2, Vec3& outMatRow3);
		
			void mFunc_ComputeConeAngleForShadowRay(ISceneObject* pObj, Vec3 pos, Vec3& outCenterDirVec, float& outConeAngle);
		};
	}
};
