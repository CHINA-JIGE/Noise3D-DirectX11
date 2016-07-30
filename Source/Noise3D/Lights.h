/***********************************************************************

                           h£ºILight interafces && lightDesc 

************************************************************************/
#pragma once

namespace Noise3D
{

	struct N_DirLightDesc
	{
		N_DirLightDesc(){ZeroMemory(this, sizeof(*this));};

		NVECTOR3 mAmbientColor;	 float		mSpecularIntensity;
		NVECTOR3 mDiffuseColor;		float			mDiffuseIntensity;
		NVECTOR3 mSpecularColor;	 float		mPad2;//memory alignment
		NVECTOR3 mDirection;			 float		mPad3;//memory alignment
	};


	struct N_PointLightDesc
	{
		N_PointLightDesc() {ZeroMemory(this, sizeof(*this));}

		NVECTOR3 mAmbientColor;		float mSpecularIntensity;
		NVECTOR3 mDiffuseColor;			float mLightingRange;
		NVECTOR3 mSpecularColor;		float mAttenuationFactor;
		NVECTOR3 mPosition;					float mDiffuseIntensity;

	};


	struct N_SpotLightDesc
	{
		N_SpotLightDesc(){ZeroMemory(this, sizeof(*this));}

		NVECTOR3 mAmbientColor;	float mSpecularIntensity;
		NVECTOR3 mDiffuseColor;		float mLightingRange;
		NVECTOR3 mSpecularColor;	float mAttenuationFactor;
		NVECTOR3 mLitAt;					float mLightingAngle;
		NVECTOR3 mPosition;				float mDiffuseIntensity;
	};


	//--------Dynamic Directional Light-------
	class IDirLightD
	{
	public:

		/*void SetAmbientColor(const NVECTOR3& color);

		void SetDiffuseColor(const NVECTOR3& color);

		void SetSpecularColor(const NVECTOR3& color);

		void SetDirection(const NVECTOR3& dir);

		void SetSpecularIntensity(float specInt);

		void SetDiffuseIntensity(float diffInt);*/

		void SetDesc(const N_DirLightDesc& desc);

		N_DirLightDesc GetDesc();

		void Destroy();


	private:
		friend IFactory<IDirLightD>;

		IDirLightD();

		~IDirLightD();

		N_DirLightDesc mLightDesc;

	};


	//--------Dynamic Point Light-------
	class IPointLightD
	{
	public:

		void SetDesc(const N_PointLightDesc& desc);

		N_PointLightDesc GetDesc();

		void Destroy();

	private:

		friend IFactory<IPointLightD>;

		IPointLightD();

		~IPointLightD();

		N_PointLightDesc mLightDesc;
	};


	//--------Dynamic Spot Light-------
	class ISpotLightD
	{
	public:

		void SetDesc(const N_SpotLightDesc& desc);

		N_SpotLightDesc GetDesc();

		void Destroy();

	private:

		friend IFactory<ISpotLightD>;

		ISpotLightD();

		~ISpotLightD();

		N_SpotLightDesc mLightDesc;

	};


	//--------Static Directional Light-------
	//immutable after initialization (so that pre-render can be applied)
	class IDirLightS
	{
	public:

		N_DirLightDesc GetDesc();

		void Destroy();

	private:
		friend IFactory<IDirLightS>;

		IDirLightS();

		~IDirLightS();

		N_DirLightDesc mLightDesc;
	};


	//--------Static Point Light-------
	class IPointLightS
	{
	public:

		N_PointLightDesc GetDesc();

		void Destroy();

	private:
		friend IFactory<IPointLightS>;

		IPointLightS();

		~IPointLightS();

		N_PointLightDesc mLightDesc;
	};


	//--------Static Spot Light-------
	class ISpotLightS
	{
	public:

		N_SpotLightDesc GetDesc();

		void Destroy();

	private:
		friend IFactory<ISpotLightS>;

		ISpotLightS();

		~ISpotLightS();

		N_SpotLightDesc mLightDesc;
	};



};