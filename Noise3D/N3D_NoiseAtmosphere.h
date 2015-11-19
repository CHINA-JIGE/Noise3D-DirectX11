/***********************************************************************

						h : NoiseAtmo

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseAtmosphere
{
public:
	friend class NoiseScene;
	friend class NoiseRenderer;


	//¹¹Ôìº¯Êý
	NoiseAtmosphere();

	void		SelfDestruction();

	void		SetFogEnabled(BOOL isEnabled);

	void		SetFogParameter(float fogNear, float fogFar, NVECTOR3 color);

	BOOL	CreateSkyDome(float fRadiusXZ, float fHeight, UINT texID);

	void		SetSkyDomeTexture(UINT texID);

	BOOL	CreateSkyBox(float fWidth, float fHeight, float fDepth, UINT cubeMapTexID);

	void		SetSkyBoxTexture(UINT cubeMapTexID);

private:
	//used to build box
	void	mFunction_Build_A_Quad(NVECTOR3 vOriginPoint, NVECTOR3 vBasisVector1, NVECTOR3 vBasisVector2, UINT StepCount1, UINT StepCount2, UINT iBaseIndex);


private:
	NoiseScene*		m_pFatherScene;

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
	UINT												mSkyDomeTextureID;
	float												mSkyBoxWidth;
	float												mSkyBoxHeight;
	float												mSkyBoxDepth;
	UINT												mSkyBoxCubeTextureID;
	std::vector<UINT>*						m_pIB_Mem_Sky;
	std::vector<N_SimpleVertex>*		m_pVB_Mem_Sky;
	ID3D11Buffer*								m_pIB_Gpu_Sky;
	ID3D11Buffer*								m_pVB_Gpu_Sky;

};