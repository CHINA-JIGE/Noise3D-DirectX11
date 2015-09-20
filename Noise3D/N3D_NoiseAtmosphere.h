/***********************************************************************

						h : NoiseAtmo

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseAtmosphere
{
public:
	friend NoiseScene;
	friend NoiseRenderer;


	//���캯��
	NoiseAtmosphere();

	void		SelfDestruction();

	BOOL	AddToRenderList();

	void		SetFogEnabled(BOOL isEnabled);

	void		SetFogParameter(float fogNear, float fogFar, NVECTOR3 color);

	BOOL	CreateSkyDome(float fRadiusXZ, float fHeight, UINT texID);

	void		SetSkyDomeTexture(UINT texID);

	BOOL	CreateSkyBox(float fWidth, float fHeight, float fDepth);

private:
	NoiseScene*		m_pFatherScene;

	//only after atmosphere was added to render list can we add apply fog effect
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
	std::vector<UINT>*						m_pIB_Mem_Sky;
	std::vector<N_SimpleVertex>*		m_pVB_Mem_Sky;
	ID3D11Buffer*								m_pIB_Gpu_Sky;
	ID3D11Buffer*								m_pVB_Gpu_Sky;

};