
/***********************************************************************

							desc£ºAdvanced Material for Path Tracer 

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

void Noise3D::GI::AdvancedGiMaterial::SetAlbedo(Color4f albedo)
{
	mAlbedo = albedo;
}

Color4f Noise3D::GI::AdvancedGiMaterial::GetAlbedo()
{
	return mAlbedo;
}
