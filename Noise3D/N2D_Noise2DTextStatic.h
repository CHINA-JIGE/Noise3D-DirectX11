/***********************************************************************

							h£ºNoise2DText

************************************************************************/

#pragma once

class _declspec(dllexport) Noise2DTextStatic:public Noise2DBasicContainerInfo,public NoiseClassLifeCycle
{
public:

	friend class NoiseScene;
	friend class NoiseRenderer;
	friend class NoiseFontManager;

	Noise2DTextStatic();

	void		SetTextColor(NVECTOR4 color);

	void		SetTextGlowColor(NVECTOR4 color);


private:

	void		Destroy();

	void		mFunction_InitGraphicObject(UINT pxWidth,UINT pxHeight,NVECTOR4 color,UINT texID);

	void		NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_UpdateGraphicObject();//extern call by renderer

private:

	NoiseFontManager*		m_pFatherFontMgr;
	NoiseGraphicObject*		m_pGraphicObj;//to store char rectangles

	NVECTOR4*		m_pTextColor;
	NVECTOR4*		m_pTextGlowColor;
	UINT					mTextureID;//to avoid performance downfall cause by GetTextureID;
	std::string*		m_pTextureName;//which bitmap texture to refer to
};
