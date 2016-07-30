/***********************************************************************

								h : NoiseAtmo

************************************************************************/

#pragma once

namespace Noise3D
{
	class _declspec(dllexport) IAtmosphere
	{
	public:
		friend  IScene;
		friend  IRenderer;

		//¹¹Ôìº¯Êý
		IAtmosphere();

		void		SetFogEnabled(BOOL isEnabled);

		void		SetFogParameter(float fogNear, float fogFar, NVECTOR3 color);

		BOOL	CreateSkyDome(float fRadiusXZ, float fHeight, std::string texName);

		void		SetSkyDomeTexture(std::string texName);

		BOOL	CreateSkyBox(float fWidth, float fHeight, float fDepth, std::string texName);

		void		SetSkyBoxTexture(std::string texName);


	protected:

		void	Destroy();


	private:
		IScene*		m_pFatherScene;

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
		std::string*									m_pSkyDomeTexName;
		float												mSkyBoxWidth;
		float												mSkyBoxHeight;
		float												mSkyBoxDepth;
		std::string*									m_pSkyBoxCubeTexName;
		std::vector<UINT>*						m_pIB_Mem_Sky;
		std::vector<N_SimpleVertex>*		m_pVB_Mem_Sky;
		ID3D11Buffer*								m_pIB_Gpu_Sky;
		ID3D11Buffer*								m_pVB_Gpu_Sky;

	};
}