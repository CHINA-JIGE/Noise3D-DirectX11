/***********************************************************************

                           h£ºNoiseTexturelManager

************************************************************************/

#pragma once


public class _declspec(dllexport) NoiseTextureManager
{
public:
	friend NoiseScene;
	friend NoiseRenderer;

	//¹¹Ôìº¯Êý
	NoiseTextureManager();


private:
	NoiseScene*		m_pFatherScene;

};
