/********************************************************

					h : Basic Render Setting 

			(cull/blend/fill modes of renderer
			are encapsuled in one class)

*********************************************************/

#pragma once

namespace Noise3D
{

	enum NOISE_FILLMODE
	{
		NOISE_FILLMODE_SOLID = D3D11_FILL_SOLID,
		NOISE_FILLMODE_WIREFRAME = D3D11_FILL_WIREFRAME,
		NOISE_FILLMODE_POINT = 0,
	};

	enum NOISE_CULLMODE
	{
		NOISE_CULLMODE_NONE = D3D11_CULL_NONE,
		NOISE_CULLMODE_BACK = D3D11_CULL_BACK,
		NOISE_CULLMODE_FRONT = D3D11_CULL_FRONT,
	};

	enum NOISE_BLENDMODE
	{
		NOISE_BLENDMODE_OPAQUE,
		NOISE_BLENDMODE_ALPHA,
		NOISE_BLENDMODE_ADDITIVE,
		NOISE_BLENDMODE_COLORFILTER,
	};

	enum NOISE_SHADEMODE
	{
		NOISE_SHADEMODE_GOURAUD,//per-vertex lighting
		NOISE_SHADEMODE_PHONG,//per-pixel lighting(advanced render technique supported)
	};

	enum NOISE_SAMPLERMODE
	{
		LINEAR
	};

	class CRenderSettingFillMode
	{
	public:

		CRenderSettingFillMode()
		{
			mFillMode = NOISE_FILLMODE_SOLID;
		};

		void SetFillMode(NOISE_FILLMODE mode) { mFillMode = mode; }

		NOISE_FILLMODE GetFillMode() { return mFillMode; }

	private:
		NOISE_FILLMODE mFillMode;
	};


	class CRenderSettingBlendMode
	{
	public:

		CRenderSettingBlendMode()
		{
			mBlendMode = NOISE_BLENDMODE_OPAQUE;
		};

		void SetBlendMode(NOISE_BLENDMODE mode) { mBlendMode = mode; }

		NOISE_BLENDMODE GetBlendMode() { return mBlendMode; };

	private:

		NOISE_BLENDMODE mBlendMode;
	};


	class CRenderSettingCullMode
	{
	public:

		CRenderSettingCullMode()
		{
			mCullMode = NOISE_CULLMODE_BACK;
		};

		void SetCullMode(NOISE_CULLMODE mode) { mCullMode = mode; }

		NOISE_CULLMODE GetCullMode() { return mCullMode; }

	private:

		NOISE_CULLMODE	mCullMode;
	};


	class CRenderSettingShadeMode
	{
	public:

		CRenderSettingShadeMode()
		{
			mShadeMode = NOISE_SHADEMODE_PHONG;
		}

		void SetShadeMode(NOISE_SHADEMODE mode) { mShadeMode = mode; }

		NOISE_SHADEMODE GetShadeMode() { return mShadeMode; }

	private:
		
		NOISE_SHADEMODE mShadeMode;
	};


	class CRenderSettingSamplerMode
	{
	public:

		CRenderSettingSamplerMode()
		{
			mSamplerMode = NOISE_SAMPLERMODE::LINEAR;
		}

		void SetSamplerMode(NOISE_SAMPLERMODE mode) { mSamplerMode = mode; }

		NOISE_SAMPLERMODE GetShadeMode() { return mSamplerMode; }

	private:

		NOISE_SAMPLERMODE mSamplerMode;
	};
}