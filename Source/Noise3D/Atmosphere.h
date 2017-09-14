/***********************************************************************

								h : NoiseAtmo

************************************************************************/

#pragma once

namespace Noise3D
{
	class  IAtmosphere
	{
	public:

		void		SetFogEnabled(bool isEnabled);

		void		SetFogParameter(float fogNear, float fogFar, NVECTOR3 color);

		void		SetSkyDomeTexture(N_UID texName);

		void		SetSkyBoxTexture(N_UID cubeMapTexName);


	private:

		friend  IRenderer;
		friend	IModelLoader;
		friend	IFactory<IAtmosphere>;

		//¹¹Ôìº¯Êý
		IAtmosphere();

		~IAtmosphere();

		bool NOISE_MACRO_FUNCTION_EXTERN_CALL mFunction_UpdateDataToVideoMem(const std::vector<N_SimpleVertex>& targetVB, const std::vector<UINT>& targetIB);

		//only after atmosphere was added to render list can we  apply fog effect
		bool					mFogHasBeenAddedToRenderList;
		bool					mFogCanUpdateToGpu;//we dont need update fog param frequently
		bool					mFogEnabled;
		float					mFogNear;
		float					mFogFar;
		NVECTOR3		mFogColor;

		//sky
		NOISE_ATMOSPHERE_SKYTYPE	mSkyType;
		float												mSkyDomeRadiusXZ;
		float												mSkyDomeHeight;
		N_UID											mSkyDomeTexName;
		float												mSkyBoxWidth;
		float												mSkyBoxHeight;
		float												mSkyBoxDepth;
		N_UID											mSkyBoxCubeTexName;
		std::vector<UINT>						mIB_Mem;
		std::vector<N_SimpleVertex>		mVB_Mem;
		ID3D11Buffer*								m_pIB_Gpu;
		ID3D11Buffer*								m_pVB_Gpu;

	};
}