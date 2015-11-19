/***********************************************************************

								h£ºNoise2DText

************************************************************************/

#pragma once
public class _declspec(dllexport) Noise2DTextDynamic:public  Noise2DBasicContainerInfo
{
public:
	friend class NoiseScene;
	friend class NoiseRenderer;
	friend class NoiseFontManager;

	Noise2DTextDynamic();

	void		SelfDestruction();

	void		SetFont(UINT fontID);

	void		SetTextAscii(std::string newText);

	void		SetTextColor(NVECTOR4 color);

	void		SetGlowColor(NVECTOR4 color);

	void		SetLineSpacingOffset(int offset);

	void		SetWordSpacingOffset(int offset);

private:

	void	mFunction_InitGraphicObject(UINT pxWidth, UINT pxHeight, NVECTOR4 color, UINT texID);
	
	void  NOISE_MACRO_FUNCTION_EXTERN_CALL	mFunction_UpdateGraphicObject();//by renderer

private:
	NoiseFontManager*	m_pFatherFontMgr;
	NoiseGraphicObject* m_pGraphicObj;//to store char rectangles
	BOOL				mIsInitialized;

	UINT					mFontID;
	UINT					mStringTextureID;
	UINT					mCharBoundarySizeX;//updated when SetFontID
	UINT					mCharBoundarySizeY;
	int					mWordSpacingOffset;
	int					mLineSpacingOffset;
	NVECTOR4*		m_pTextColor;
	NVECTOR4*		m_pTextGlowColor;
	std::string*		m_pTextureName;//which bitmap texture to refer to
	std::string*		m_pTextContent;//the target "string"
};
