/********************************************************

					h : Basic Render Setting 

			(cull/blend/fill modes of renderer
			are encapsuled in one class)

*********************************************************/

#pragma once

namespace Noise3D
{

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

};