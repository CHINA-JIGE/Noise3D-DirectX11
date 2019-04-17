
/***********************************************************

								GI: light manager
			manager of lights for Global Illumination 
			e.g. Area Light. (might owns a basic shape)
			accurate light with radiometric measures

**************************************************************/

#pragma once

namespace Noise3D
{
	namespace GI
	{
		class /*_declspec(dllexport)*/ GiLightManager:
			IFactoryEx<AreaLight>
		{
		public:

		private:

		};
	}
}