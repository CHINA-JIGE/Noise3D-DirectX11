
/***********************************************************************

											  cpp£º Lights

			Desc£ºdef of light interfaces and light description structure

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::Ut;

//-------------------Base Light--------------------------
IBaseLight::IBaseLight()
{

}

void IBaseLight::SetAmbientColor(const NVECTOR3 & color)
{
	mBaseLightDesc.ambientColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetDiffuseColor(const NVECTOR3 & color)
{
	mBaseLightDesc.diffuseColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetSpecularColor(const NVECTOR3 & color)
{
	mBaseLightDesc.specularColor = Clamp(color, NVECTOR3(0.0f, 0.0f, 0.0f), NVECTOR3(1.0f, 1.0f, 1.0f));
}

void IBaseLight::SetSpecularIntensity(float specInt)
{
	mBaseLightDesc.specularIntensity = Clamp(specInt, 0.0f, 100.0f);
}

void IBaseLight::SetDiffuseIntensity(float diffInt)
{
	mBaseLightDesc.diffuseIntensity = Clamp(diffInt, 0.0f, 100.0f);
}

void IBaseLight::SetDesc(const N_CommonLightDesc & desc)
{
	SetDiffuseColor(desc.diffuseColor);
	SetAmbientColor(desc.ambientColor);
	SetSpecularColor(desc.specularColor);
	SetSpecularIntensity(desc.specularIntensity);
	SetDiffuseIntensity(desc.diffuseIntensity);
}

void IBaseLight::GetDesc(N_CommonLightDesc & outDesc)
{
	outDesc.ambientColor = mBaseLightDesc.ambientColor;
	outDesc.diffuseColor = mBaseLightDesc.diffuseColor;
	outDesc.specularColor = mBaseLightDesc.specularColor;
	outDesc.diffuseIntensity = mBaseLightDesc.diffuseIntensity;
	outDesc.specularIntensity = mBaseLightDesc.specularIntensity;
}



//--------------------DYNAMIC DIR LIGHT------------------
DirLight::DirLight()
{
	mLightDesc.specularIntensity = 1.0f;
	mLightDesc.direction = NVECTOR3(1.0f, 0, 0);
	mLightDesc.diffuseIntensity = 0.5;
}

DirLight::~DirLight()
{

}

void DirLight::SetDirection(const NVECTOR3& dir)
{
	//the length of directional vector must be greater than 0
	if (!(dir.x == 0 && dir.y == 0 && dir.z == 0))
	{
		mLightDesc.direction = dir;
	}
}

NVECTOR3 Noise3D::DirLight::GetDirection()
{
	return mLightDesc.direction;
}

NVECTOR3 Noise3D::DirLight::GetDirection_WorldSpace()
{
	NMATRIX mat = ISceneObject::GetAttachedSceneNode()->EvalWorldTransform().GetRotationMatrix();
	NVECTOR3 vec = AffineTransform::TransformVector_MatrixMul(mLightDesc.direction, mat);
	return vec;
}

void DirLight::SetDesc(const N_DirLightDesc & desc)
{
	IBaseLight::SetDesc(desc);//only modify the common part
	SetDirection(desc.direction);//modify extra part
}

N_DirLightDesc DirLight::GetDesc()
{
	//fill in the common attribute part
	IBaseLight::GetDesc(mLightDesc);
	return mLightDesc;
}

N_DirLightDesc Noise3D::DirLight::GetDesc_TransformedToWorld()
{
	//transform dir light's direction before update info
	N_DirLightDesc desc = DirLight::GetDesc();
	desc.direction = DirLight::GetDirection_WorldSpace();
	return desc;
}

N_AABB Noise3D::DirLight::GetLocalAABB()
{
	return N_AABB();
}

N_AABB Noise3D::DirLight::ComputeWorldAABB_Accurate()
{
	return N_AABB();
}

N_AABB Noise3D::DirLight::ComputeWorldAABB_Fast()
{
	return N_AABB();
}

NOISE_SCENE_OBJECT_TYPE Noise3D::DirLight::GetObjectType()const
{
	return NOISE_SCENE_OBJECT_TYPE::LIGHT;
}

//overriden virtual function
bool Noise3D::DirLight::mFunction_InitShadowMap(N_SHADOW_MAPPING_PARAM smParam)
{
	IShadowCaster::mShadowMapParam = smParam;

	//-----------DSV of shadow map-------------
	D3D11_TEXTURE2D_DESC DSBufferDesc;
	DSBufferDesc.Width = smParam.pixelWidth;
	DSBufferDesc.Height = smParam.pixelHeight;
	DSBufferDesc.MipLevels = 1;
	DSBufferDesc.ArraySize = 1;
	DSBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;//it will be then cast to 2 format later
	DSBufferDesc.SampleDesc.Count = 1;//if MSAA enabled, RT/DS buffer must have same quality
	DSBufferDesc.SampleDesc.Quality =0;
	DSBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DSBufferDesc.CPUAccessFlags = 0;
	DSBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;//it's also used as shader resource
	DSBufferDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilTexture;
	HRESULT hr = Noise3D::D3D::g_pd3dDevice11->CreateTexture2D(&DSBufferDesc, 0, &pDepthStencilTexture);
	HR_DEBUG(hr, "Light: failed to create shadow map texture 2d.");

	//create DSV (for pass 1: shadow map generation)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DXGI_FORMAT_R32_TYPELESS;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = Noise3D::D3D::g_pd3dDevice11->CreateDepthStencilView(pDepthStencilTexture, &dsvDesc, &m_pShadowMapPass1_DSV);
	//remember to delete other resources if init failed in the middle
	Ut::Debug_ComPtrBatchDestructionWithHResultDebug(hr, "Light: failed to create shadow map DSV.", 
		1, pDepthStencilTexture);

	//create SRV (for pass 2: shadowing & sampling shadow map)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;// DXGI_FORMAT_R32_TYPELESS;// DXGI_FORMAT_R24_UNORM_X8_TYPELESS;//same pixel format as DSV in pass 1
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = DSBufferDesc.MipLevels;
	hr = Noise3D::D3D::g_pd3dDevice11->CreateShaderResourceView(pDepthStencilTexture, &srvDesc, &m_pShadowMapPass2_SRV);
	//remember to delete other resources if init failed in the middle
	Ut::Debug_ComPtrBatchDestructionWithHResultDebug(hr, "Light: failed to create shadow map DSV.", 
		2, pDepthStencilTexture, m_pShadowMapPass1_DSV);

	pDepthStencilTexture->Release();
	return true;
}




//--------------------DYNAMIC POINT LIGHT------------------
PointLight::PointLight()
{
	mLightDesc.specularIntensity = 1.0f;
	mLightDesc.attenuationFactor = 0.05f;
	mLightDesc.lightingRange = 100.0f;
	mLightDesc.diffuseIntensity = 0.5;
}

PointLight::~PointLight()
{
}

void PointLight::SetPosition(const NVECTOR3 & pos)
{
	mLightDesc.position = pos;
}

NVECTOR3 Noise3D::PointLight::GetPostion()
{
	return mLightDesc.position;
}

NVECTOR3 Noise3D::PointLight::GetPosition_WorldSpace()
{
	NMATRIX mat = ISceneObject::GetAttachedSceneNode()->EvalWorldTransform().GetAffineTransformMatrix();
	NVECTOR3 vec = AffineTransform::TransformVector_MatrixMul(mLightDesc.position, mat);
	return vec;
}

void PointLight::SetAttenuationFactor(float attFactor)
{
	mLightDesc.attenuationFactor = Clamp(attFactor,0.0f,1.0f);
}

void PointLight::SetLightingRange(float range)
{
	mLightDesc.lightingRange = Clamp(range, 0.0f, 10000000.0f);
}

void PointLight::SetDesc(const N_PointLightDesc & desc)
{
	IBaseLight::SetDesc(desc);
	SetPosition(desc.position);
	SetAttenuationFactor(desc.attenuationFactor);
	SetLightingRange(desc.lightingRange);
}

N_PointLightDesc PointLight::GetDesc()
{
	//fill in the common attribute part
	IBaseLight::GetDesc(mLightDesc);
	return mLightDesc;
}

N_PointLightDesc Noise3D::PointLight::GetDesc_TransformedToWorld()
{
	N_PointLightDesc desc = PointLight::GetDesc();
	desc.position = PointLight::GetPosition_WorldSpace();
	return desc;
}

N_AABB Noise3D::PointLight::GetLocalAABB()
{
	return N_AABB();
}

N_AABB Noise3D::PointLight::ComputeWorldAABB_Accurate()
{
	return N_AABB();
}

N_AABB Noise3D::PointLight::ComputeWorldAABB_Fast()
{
	return N_AABB();
}

NOISE_SCENE_OBJECT_TYPE Noise3D::PointLight::GetObjectType()const
{
	return NOISE_SCENE_OBJECT_TYPE::LIGHT;
}



//--------------------DYNAMIC SPOT LIGHT------------------

SpotLight::SpotLight()
{
	mLightDesc.specularIntensity = 1.0f;
	mLightDesc.attenuationFactor = 1.0f;
	mLightDesc.lightingRange = 100.0f;
	mLightDesc.lightingAngle = Ut::PI / 4.0f;
	mLightDesc.diffuseIntensity = 0.5;
	mLightDesc.lookAt = NVECTOR3(1.0f, 0, 0);
	mLightDesc.position = NVECTOR3(0, 0, 0);
}

SpotLight::~SpotLight()
{
}

void SpotLight::SetPosition(const NVECTOR3 & pos)
{
	NVECTOR3 deltaVec = pos - mLightDesc.lookAt;

	//pos and lookAt can't superpose
	if (!(deltaVec.x == 0 && deltaVec.y == 0 && deltaVec.z == 0))
	{
		mLightDesc.position = pos;
	}
}

NVECTOR3 Noise3D::SpotLight::GetPosition()
{
	return mLightDesc.position;
}

NVECTOR3 Noise3D::SpotLight::GetPosition_WorldSpace()
{
	NMATRIX mat = ISceneObject::GetAttachedSceneNode()->EvalWorldTransform().GetAffineTransformMatrix();
	NVECTOR3 vec = AffineTransform::TransformVector_MatrixMul(mLightDesc.position, mat);
	return vec;
}

void SpotLight::SetAttenuationFactor(float attFactor)
{
	mLightDesc.attenuationFactor = Clamp(attFactor,0.0f,1.0f);
}

void SpotLight::SetLookAt(const NVECTOR3 & vLitAt)
{
	NVECTOR3 deltaVec = vLitAt - mLightDesc.position;

	//pos and lookAt can't superpose
	if (!(deltaVec.x == 0 && deltaVec.y == 0 && deltaVec.z == 0))
	{
		mLightDesc.lookAt = vLitAt;
	}
}

NVECTOR3 Noise3D::SpotLight::GetLookAt()
{
	return mLightDesc.lookAt;
}

NVECTOR3 Noise3D::SpotLight::GetLookAt_WorldSpace()
{
 	NMATRIX mat = ISceneObject::GetAttachedSceneNode()->EvalWorldTransform().GetAffineTransformMatrix();
	NVECTOR3 vec = AffineTransform::TransformVector_MatrixMul(mLightDesc.lookAt, mat);
	return vec;
}

void SpotLight::SetLightingAngle(float coneAngle_Rad)
{
	// i'm not sure...but spot light should have a cone angle smaller than ¦Ð...??
	mLightDesc.lightingAngle = Clamp(coneAngle_Rad, 0.0f, Ut::PI-0.001f);
}

void SpotLight::SetLightingRange(float range)
{
	mLightDesc.lightingRange = Clamp(range, 0.0f, 10000000.0f);
}

void SpotLight::SetDesc(const N_SpotLightDesc & desc)
{
	IBaseLight::SetDesc(desc);
	SetPosition(desc.position);
	SetLookAt(desc.lookAt);
	SetAttenuationFactor(desc.attenuationFactor);
	SetLightingRange(desc.lightingRange);
	SetLightingAngle(desc.lightingAngle);
}

N_SpotLightDesc SpotLight::GetDesc()
{
	//fill in the common attribute part
	IBaseLight::GetDesc(mLightDesc);
	return mLightDesc;
}

N_SpotLightDesc Noise3D::SpotLight::GetDesc_TransformedToWorld()
{
	N_SpotLightDesc desc = SpotLight::GetDesc();
	NMATRIX mat = ISceneObject::GetAttachedSceneNode()->EvalWorldTransform().GetAffineTransformMatrix();
	desc.position = AffineTransform::TransformVector_MatrixMul(mLightDesc.position, mat);
	desc.lookAt = AffineTransform::TransformVector_MatrixMul(mLightDesc.lookAt, mat);
	return desc;
}

N_AABB Noise3D::SpotLight::GetLocalAABB()
{
	return N_AABB();
}

N_AABB Noise3D::SpotLight::ComputeWorldAABB_Accurate()
{
	return N_AABB();
}

N_AABB Noise3D::SpotLight::ComputeWorldAABB_Fast()
{
	return N_AABB();
}

NOISE_SCENE_OBJECT_TYPE Noise3D::SpotLight::GetObjectType()const
{
	return NOISE_SCENE_OBJECT_TYPE::LIGHT;
}

