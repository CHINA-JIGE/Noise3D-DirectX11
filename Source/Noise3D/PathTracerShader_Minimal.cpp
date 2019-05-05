/***********************************************************

					Path Tracer Shader: Minimal

			Noise3D::PathTracer's soft shader "hello world", 
			for demonstration and test usage

***********************************************************/
#include "Noise3D.h"
#include "Noise3D_InDevHeader.h"

using namespace Noise3D;

void Noise3D::GI::PathTracerShader_MinimalDemo::ClosestHit(const N_TraceRayParam& param, const N_RayHitInfoForPathTracer & hitInfo, N_TraceRayPayload & in_out_payload)
{
	//in_out_payload.radiance = GI::Radiance(1.0f, 0, 0);
	//in_out_payload.radiance = GI::Radiance(abs(hitInfo.normal.x), abs(hitInfo.normal.y), abs(hitInfo.normal.z));
	Vec3 rgb = hitInfo.normal;
	//Vec3 rgb = Vec3(hitInfo.texcoord.x, hitInfo.texcoord.y, 0.0f);
	/*GI::PbrtMaterial* pMat = nullptr;
	if (hitInfo.pHitObj->GetObjectType() == NOISE_SCENE_OBJECT_TYPE::MESH)
	{
		pMat = static_cast<Mesh*>(hitInfo.pHitObj)->GetPbrtMaterial(hitInfo.triangleIndex);
	}
	else
	{
		pMat = hitInfo.pHitObj->GetPbrtMaterial();
	}

	Vec3 rgb;
	if (pMat != nullptr)
	{
		GI::N_PbrtMatDesc mat = pMat->GetDesc();
		rgb = Vec3(mat.albedo.x, mat.albedo.y, mat.albedo.z);
		if (mat.pAlbedoMap != nullptr)
		{
			Color4f c = mat.pAlbedoMap->SamplePixelBilinear(hitInfo.texcoord);
			rgb = Vec3(c.x, c.y, c.z);
		}
	}*/
	
	if (rgb.x < 0)rgb.x *= -0.5f;
	if (rgb.y < 0)rgb.y *= -0.5f;
	if (rgb.z < 0)rgb.z *= -0.5f;
	in_out_payload.radiance = GI::Radiance(rgb);
}

void Noise3D::GI::PathTracerShader_MinimalDemo::Miss(const N_TraceRayParam & param, N_TraceRayPayload & in_out_payload)
{
	in_out_payload.radiance = GI::Radiance(0, 0, 0);
}
