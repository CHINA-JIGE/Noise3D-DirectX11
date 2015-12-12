/***********************************************************************

							h£ºNoise2DText

************************************************************************/

#pragma once

class _declspec(dllexport) Noise2DTextStatic:
	public Noise2DBasicContainerInfo,
	public NoiseClassLifeCycle,
	public Noise2DBasicTextInfo
{
public:

	friend class NoiseScene;
	friend class NoiseRenderer;
	friend class NoiseFontManager;

	Noise2DTextStatic();

	//void		SetTextColor(NVECTOR4 color);

	//void		SetTextGlowColor(NVECTOR4 color);


private:

	void		Destroy();

	void		mFunction_InitGraphicObject(UINT pxWidth,UINT pxHeight,NVECTOR4 color,UINT texID);

	void		NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_UpdateGraphicObject();//extern call by renderer

private:

	UINT					mStringTextureID;//to avoid performance overhead caused by GetTextureID;
	std::string*		m_pTextureName;//which bitmap texture to refer to
};
