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
		NOISE_BLENDMODE_OPAQUE = 0,
		NOISE_BLENDMODE_ALPHA = 1,
		NOISE_BLENDMODE_ADDITIVE = 2,
		NOISE_BLENDMODE_COLORFILTER = 3,
	};

	enum NOISE_SHADEMODE
	{
		NOISE_SHADEMODE_GOURAUD = 0,//per-vertex lighting
		NOISE_SHADEMODE_PHONG = 1,//per-pixel lighting
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
			mShadeMode =NOISE_SHADEMODE_PHONG;
		}

		void SetShadeMode(NOISE_SHADEMODE mode) { mShadeMode = mode; }

		NOISE_SHADEMODE GetShadeMode() { return mShadeMode; }

	private:
		
		NOISE_SHADEMODE mShadeMode;
	};
}