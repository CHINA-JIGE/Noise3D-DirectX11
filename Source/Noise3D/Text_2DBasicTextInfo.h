
/***********************************************************************

							CLASS:  Noise2DTextCommonInfo

	To Abstract some common attribute of TextDynamic/Static
	such that at least In renderer I can avoid template(which in this case is type-unsafe)
	(just find out there is to many identical logic which can be 
	avoided by this base class)
************************************************************************/

#pragma once

namespace Noise3D
{

	//a base class that abstract common text attribute and op
	class /*_declspec(dllexport)*/ IBasicTextInfo
		: public CRenderSettingBlendMode
		{

		public:
			IBasicTextInfo();

			void		SetTextColor(NVECTOR4 color) { *m_pTextColor = color; };

			void		SetGlowColor(NVECTOR4 color) { *m_pTextGlowColor = color; };

			NVECTOR4 GetTextColor() { return *m_pTextColor; };

			NVECTOR4 GetTextGlowColor() { return *m_pTextGlowColor; };
		protected:

			friend class IRenderModuleForText;

			virtual	void	mFunction_UpdateGraphicObject() = 0;//require implementation

			IGraphicObject* m_pGraphicObj;//to store char rectangles

			NVECTOR4*		m_pTextColor;

			NVECTOR4*		m_pTextGlowColor;
		};
}