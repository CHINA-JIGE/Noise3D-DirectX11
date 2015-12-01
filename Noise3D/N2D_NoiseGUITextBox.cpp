
/***********************************************************************

								Àà£ºNOISE GUI Label

						Description : ineditable text box

************************************************************************/

#include "Noise3D.h"


NoiseGUITextBox::NoiseGUITextBox()
{
	mIsEnabled = TRUE;
	m_pGraphicObj = new NoiseGraphicObject;
}

void NoiseGUITextBox::SetEnabled(BOOL isEnabled)
{
	mIsEnabled =isEnabled;
}
BOOL NoiseGUITextBox::IsEnabled()
{
	return mIsEnabled;
};


void NoiseGUITextBox::Destroy()
{
	m_pGraphicObj->SelfDestruction();
}
