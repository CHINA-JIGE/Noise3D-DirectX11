/***********************************************************************

								h : NoiseAtmo

************************************************************************/

#pragma once

namespace Noise3D
{
	enum NOISE_ATMOSPHERE_SKYTYPE
	{
		NOISE_ATMOSPHERE_SKYTYPE_BOX = 0,
		NOISE_ATMOSPHERE_SKYTYPE_DOME = 1,
		NOISE_ATMOSPHERE_SKYTYPE_INVALID = UINT_MAX,
	};

	class  /*_declspec(dllexport)*/ Atmosphere
	{
	public:

		void		SetFogEnabled(bool isEnabled);

		bool		IsFogEnabled();

		void		SetFogParameter(float fogNear, float fogFar, NVECTOR3 color);

		void		SetSkyDomeTexture(N_UID texName);

		void		SetSkyBoxTexture(N_UID cubeMapTexName);

		N_UID	GetSkyTextureUID();

		NOISE_ATMOSPHERE_SKYTYPE GetSkyType();

		//SET xxx is not available here because the size of sky box is fixed
		//when MeshLoader load the real model of sky
		float	GetSkyboxWidth();

		float	GetSkyboxHeight();

		float	GetSkyboxDepth();


	private:

		friend  class IRenderModuleForAtmosphere;
		friend  class MeshLoader;
		friend	IFactory<Atmosphere>;

		//¹¹Ôìº¯Êý
		Atmosphere();

		~Atmosphere();

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_CreateGpuBufferAndUpdateData(const std::vector<N_SimpleVertex>& targetVB,const  std::vector<UINT>& targetIB);

		//only after atmosphere was added to render list can we  apply fog effect
		bool					mFogCanUpdateToGpu;//we dont need update fog param frequently
		bool					mFogEnabled;
		float					mFogNear;
		float					mFogFar;
		NVECTOR3		mFogColor;

		//sky
		NOISE_ATMOSPHERE_SKYTYPE	mSkyType;
		float		mSkyDomeRadiusXZ;
		float		mSkyDomeHeight;
		N_UID	mSkyDomeTexName;
		float		mSkyBoxWidth;
		float		mSkyBoxHeight;
		float		mSkyBoxDepth;
		N_UID	mSkyBoxCubeTexName;
		std::vector<UINT>	mIB_Mem;
		std::vector<N_SimpleVertex>	mVB_Mem;
		ID3D11Buffer*	 m_pIB_Gpu;
		ID3D11Buffer*	 m_pVB_Gpu;

	};
}