
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
			virtual T Eval(const NVECTOR3& dir) = 0;
		};

		//concrete spherical func implementation based on texture sampling in spherical mapping way
		class ISphericalFunc_Texture2dSampler : public ISphericalFunc<NColor4f>
		{
		public:

			void SetTexturePtr(Texture2D* pTex);

			//evaluate a spherical function value by sampling a texture
			virtual NColor4f Eval(const NVECTOR3& dir) override;

		private:

			Texture2D* m_pTex;
		};

		//concrete spherical func implementation based on cube map
		class ISphericalFunc_CubeMapSampler : public ISphericalFunc<NColor4f>
		{
		public:

			void SetTexturePtr(TextureCubeMap* pTex);

			//evaluate a spherical function value by sampling a texture
			virtual NColor4f Eval(const NVECTOR3& dir) override;

		private:

			TextureCubeMap* m_pTex;
		};

	}
}