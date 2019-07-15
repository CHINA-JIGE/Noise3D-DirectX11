
/***********************************************************************

								h£º ISphericalFunc
		1.an interface that must be implemented as the input of SH projector
		2.several default Spherical Func is implemented

************************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		//an interface that user can implement to pass in as the input of data(spherical function)
		template <typename T>
		struct ISphericalFunc
		{
			virtual T Eval(const Vec3& dir) = 0;
		};

		//concrete spherical func implementation based on texture sampling in spherical mapping way
		class Texture2dSampler_Spherical : public ISphericalFunc<Color4f>
		{
		public:

			Texture2dSampler_Spherical();

			void SetFilterMode(bool isBilinear);

			void SetTexturePtr(Texture2D* pTex);

			//evaluate a spherical function value by sampling a texture
			virtual Color4f Eval(const Vec3& dir) override;

		private:

			bool mIsBilinear;

			Texture2D* m_pTex;
		};

		//concrete spherical func implementation based on cube map
		class CubeMapSampler : public ISphericalFunc<Color4f>
		{
		public:

			void SetTexturePtr(TextureCubeMap* pTex);

			//evaluate a spherical function value by sampling a texture
			virtual Color4f Eval(const Vec3& dir) override;

		private:

			TextureCubeMap* m_pTex;
		};

		//concrete spherical func implementation based on texture sampling in spherical mapping way
		class Texture2dSamplerForSHProjection : public ISphericalFunc<Color4f>
		{
		public:

			Texture2dSamplerForSHProjection();

			void SetTexturePtr(Texture2D* pTex);

			//evaluate a spherical function value by sampling a texture
			virtual Color4f Eval(const Vec3& dir) override;

		private:

			bool mIsBilinear;

			Texture2D* m_pTex;
		};

	}
}