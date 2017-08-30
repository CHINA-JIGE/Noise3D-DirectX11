/***********************************************************************

								h : NoiseAtmo

************************************************************************/

#pragma once

namespace Noise3D
{
	class  IAtmosphere
	{
	public:

		void		SetFogEnabled(BOOL isEnabled);

		void		SetFogParameter(float fogNear, float fogFar, NVECTOR3 color);

		BOOL	CreateSkyDome(float fRadiusXZ, float fHeight, N_UID texName);

		void		SetSkyDomeTexture(N_UID texName);

		BOOL	CreateSkyBox(float fWidth, float fHeight, float fDepth, N_UID texName);

		void		SetSkyBoxTexture(std::string texName);


	private:
		friend  IRenderer;

		friend IFactory<IAtmosphere>;

		//¹¹Ôìº¯Êý
		IAtmosphere();

		~IAtmosphere();

		IGeometryMeshGenerator mMeshGenerator;
		//only after atmosphere was added to render list can we  apply fog effect
		BOOL				mFogHasBeenAddedToRenderList;
		BOOL				mFogCanUpdateToGpu;//we dont need update fog param frequently
		BOOL				mFogEnabled;
		float					mFogNear;
		float					mFogFar;
		NVECTOR3*		m_pFogColor;

		//sky
		NOISE_ATMOSPHERE_SKYTYPE		mSkyType;
		float												mSkyDomeRadiusXZ;
		float												mSkyDomeHeight;
		N_UID*									m_pSkyDomeTexName;
		float												mSkyBoxWidth;
		float												mSkyBoxHeight;
		float												mSkyBoxDepth;
		N_UID*									m_pSkyBoxCubeTexName;
		std::vector<UINT>*						m_pIB_Mem_Sky;
		std::vector<N_SimpleVertex>*		m_pVB_Mem_Sky;
		ID3D11Buffer*								m_pIB_Gpu_Sky;
		ID3D11Buffer*								m_pVB_Gpu_Sky;

	};
}