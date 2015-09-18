/***********************************************************************

						h : NoiseAtmo

************************************************************************/

#pragma once

enum NOISE_ATMOSPHERE_SKYTYPE
{
	NOISE_ATMOSPHERE_SKYTYPE_BOX = 0,
	NOISE_ATMOSPHERE_SKYTYPE_SPHERE = 1,
};


public class _declspec(dllexport) NoiseAtmosphere
{
public:
	friend NoiseScene;
	friend NoiseRenderer;


	//¹¹Ôìº¯Êý
	NoiseAtmosphere();

	void		SelfDestruction();

	BOOL	AddToRenderList();

	void		SetFogEnabled(BOOL isEnabled);

	void		SetFogParameter(float fogNear, float fogFar, NVECTOR3 color);

	BOOL	CreateSkyDome(float fRadiusXZ, float fHeight, UINT texID);

	BOOL	CreateSkyBox(float fWidth, float fHeight, float fDepth);

private:
	NoiseScene*		m_pFatherScene;

	//we dont need to update atmo param frequently
	BOOL				mCanUpdateAmtosphere;
	
	//only after atmosphere was added to render list can we 
	BOOL				mFogHasBeenAddedToRenderList;
	BOOL				mFogEnabled;
	float					mFogNear;
	float					mFogFar;
	NVECTOR3*		m_pFogColor;

	//skybox
	float												mSkyDomeRadiusXZ;
	float												mSkyDomeHeight;
	UINT												mSkyDomeTextureID;
	std::vector<UINT>*						m_pIB_Mem_Sky;
	std::vector<N_SimpleVertex>*		m_pVB_Mem_Sky;
	ID3D11Buffer*								m_pIB_Gpu_Sky;
	ID3D11Buffer*								m_pVB_Gpu_Sky;

};