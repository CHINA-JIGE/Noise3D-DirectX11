
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

			void UniformSphericalVec_Cone(Vec3 normal, float maxAngle, int sampleCount, std::vector<Vec3>& outVecList);

			Vec3 UniformSphericalVec_Hemisphere(Vec3 normal);

			void UniformSphericalVec_Hemisphere(Vec3 normal, int sampleCount, std::vector<Vec3>& outVecList);

			void UniformSphericalVec_ShadowRays(Vec3 pos, ISceneObject* pObj, int sampleCount, std::vector<Vec3>& outVecList);

		private:

			static std::default_random_engine mRandomEngine;

			static std::uniform_real_distribution<float> mCanonicalDist;

			static std::uniform_real_distribution<float> mNormalizedDist;

			Vec3 mFunc_UniformSphericalVecGen_AzimuthalToDir(float theta, float phi);
		};
	}
};
