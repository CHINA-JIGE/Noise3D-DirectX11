/***********************************************************************

                           h: Advanced material for GI Renderer

************************************************************************/
#pragma once

namespace Noise3D
{
	namespace GI
	{
		//material specifically for PathTracer
		class AdvancedGiMaterial
		{
		public:

			//optimization will be made for PathTracer, in which texture ptr are access frequently.
			//textures are access directly throught ptr.

			//(2019.4.5)one day, reference counting of Texture/Material should be implemented.
			//and those who owns resource X will be inherited from sth like IResourceOwner<X>
			void SetAlbedo(Color4f albedo);

			Color4f GetAlbedo();

		private:

			Color4f mAlbedo;


		};
	}
}