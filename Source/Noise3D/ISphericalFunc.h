
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
		class ISphericalMappingTextureSampler : public ISphericalFunc<NVECTOR3>
		{
		public:

			void SetTexture(ITexture* pTex);

			//evaluate a spherical function value by sampling a texture
			virtual NVECTOR3 Eval(const NVECTOR3& dir) override;

		private:

			ITexture* m_pTex;
		};

	}
}