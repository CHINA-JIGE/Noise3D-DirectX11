/***********************************************************************

                           h£ºILight interafces && lightDesc 

************************************************************************/
#pragma once

namespace Noise3D
{
	//LIGHT description : just combination of data
	//LIGHT interfaces : provide interfaces for the user to interact


	struct N_CommonLightDesc
	{
		N_CommonLightDesc() { ZeroMemory(this, sizeof(*this)); }
		NVECTOR3	mAmbientColor;		float				mSpecularIntensity;
		NVECTOR3	mDiffuseColor;			float				mDiffuseIntensity;
		NVECTOR3	mSpecularColor;	//4 bytes left to pad to fulfill 128 bytes alignment
	};


	//don't forget graphic memory's 128 bit alignment
	struct N_DirLightDesc
		:public N_CommonLightDesc
	{
		N_DirLightDesc() { ZeroMemory(this, sizeof(*this)); };

		/*NVECTOR3	mAmbientColor;		float				mSpecularIntensity;
		NVECTOR3	mDiffuseColor;				float				mDiffuseIntensity;
		NVECTOR3	mSpecularColor;*/		float		mPad2;
		NVECTOR3 mDirection;					float		mPad3;
	};


	//don't forget graphic memory's 128 bit alignment
	struct N_PointLightDesc 
		:public N_CommonLightDesc
	{
		N_PointLightDesc() {ZeroMemory(this, sizeof(*this));}

		/*NVECTOR3	mAmbientColor;		float				mSpecularIntensity;
		NVECTOR3	mDiffuseColor;				float				mDiffuseIntensity;
		NVECTOR3	mSpecularColor;*/		float		mAttenuationFactor;
		NVECTOR3 mPosition;						float		mLightingRange;

	};


	//don't forget graphic memory's 128 bit alignment
	struct N_SpotLightDesc
		:public N_CommonLightDesc
	{
		N_SpotLightDesc(){ZeroMemory(this, sizeof(*this));}

		/*NVECTOR3 mAmbientColor;		float mSpecularIntensity;
		NVECTOR3 mDiffuseColor;			float mDiffuseIntensity;
		NVECTOR3 mSpecularColor;*/	float mAttenuationFactor;
		NVECTOR3 mLitAt;						float mLightingAngle;
		NVECTOR3 mPosition;					float mLightingRange;
	};


	//----------------------BASE LIGHT-------------------------------
	class IBaseLight
	{
	public:

		void SetAmbientColor(const NVECTOR3& color);

		void SetDiffuseColor(const NVECTOR3& color);

		void SetSpecularColor(const NVECTOR3& color);

		void SetSpecularIntensity(float specInt);

		void SetDiffuseIntensity(float diffInt);

	protected:

		//invoked by derived Light,not by user
		void	SetDesc(const N_CommonLightDesc& desc);

		//invoked by derived Light,not by user
		void GetDesc(N_CommonLightDesc& outDesc);

	private:
		N_CommonLightDesc mBaseLightDesc;
	};


	//---------------------Dynamic Directional Light------------------
	class IDirLightD : public IBaseLight
	{
	public:

		void	SetDirection(const NVECTOR3& dir);

		void SetDesc(const N_DirLightDesc& desc);//many CLAMP op happens in this

		N_DirLightDesc GetDesc();


	private:
		friend IFactory<IDirLightD>;

		IDirLightD();

		~IDirLightD();

		N_DirLightDesc mLightDesc;

	};


	//-----------------------Dynamic Point Light--------------------
	class IPointLightD : public IBaseLight
	{
	public:

		void SetPosition(const NVECTOR3& pos);

		void SetAttenuationFactor(float attFactor);

		void	SetLightingRange(float range);

		void SetDesc(const N_PointLightDesc& desc);//many CLAMP op happens in this

		N_PointLightDesc GetDesc();

	private:

		friend IFactory<IPointLightD>;

		IPointLightD();

		~IPointLightD();

		N_PointLightDesc mLightDesc;
	};


	//-----------------------Dynamic Spot Light------------------
	class ISpotLightD:public IBaseLight
	{
	public:

		void SetPosition(const NVECTOR3& pos);

		void SetAttenuationFactor(float attFactor);

		void	SetLitAt(const NVECTOR3& vLitAt);

		void	SetLightingAngle(float coneAngle_Rad);

		void	SetLightingRange(float range);

		void SetDesc(const N_SpotLightDesc& desc);//many CLAMP op happens in this

		N_SpotLightDesc GetDesc();

	private:

		friend IFactory<ISpotLightD>;

		ISpotLightD();

		~ISpotLightD();

		N_SpotLightDesc mLightDesc;

	};




	//------------------Static Directional Light------------------
	//immutable after initialization (so that pre-render can be applied)
	class IDirLightS
	{
	public:

		N_DirLightDesc GetDesc();

	private:

		friend class ILightManager;

		friend IFactory<IDirLightS>;

		IDirLightS();

		~IDirLightS();

		//the IFactory<> didn't accept constructor with parameters... 
		//thus an additional Init() func should be implemented and invoked by lightMgr
		BOOL	mFunction_Init(const N_DirLightDesc& desc);

		N_DirLightDesc mLightDesc;
	};


	//----------------------Static Point Light------------------
	class IPointLightS
	{
	public:

		N_PointLightDesc GetDesc();

	private:

		friend class ILightManager;

		friend IFactory<IPointLightS>;

		IPointLightS();

		~IPointLightS();

		BOOL	mFunction_Init(const N_PointLightDesc& desc);

		N_PointLightDesc mLightDesc;
	};


	//------------------Static Spot Light--------------------
	class ISpotLightS
	{
	public:

		N_SpotLightDesc GetDesc();

	private:

		friend class ILightManager;

		friend IFactory<ISpotLightS>;

		ISpotLightS();

		~ISpotLightS();

		BOOL	mFunction_Init(const N_SpotLightDesc& desc);

		N_SpotLightDesc mLightDesc;
	};



};