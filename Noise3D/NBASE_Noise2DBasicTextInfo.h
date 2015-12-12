
/***********************************************************************

							CLASS:  Noise2DTextCommonInfo

	To Abstract some common attribute of TextDynamic/Static
	such that at least In renderer I can avoid template(which in this case is type-unsafe)
	(just find out there is to many identical logic which can be 
	avoided by this base class)
************************************************************************/

#pragma once

class NoiseGraphicObject;
class NoiseFontManager;

//a base class that provide general life cycle management
class _declspec(dllexport) Noise2DBasicTextInfo
{
	friend class NoiseRenderer;

public:
	Noise2DBasicTextInfo();

	void		SetTextColor(NVECTOR4 color) { *m_pTextColor = color; };

	void		SetGlowColor(NVECTOR4 color) { *m_pTextGlowColor = color; };


protected:

	virtual	void	mFunction_UpdateGraphicObject() = 0;//require implementation

	NoiseFontManager*	m_pFatherFontMgr;

	NoiseGraphicObject* m_pGraphicObj;//to store char rectangles

	NVECTOR4*		m_pTextColor;

	NVECTOR4*		m_pTextGlowColor;
};

