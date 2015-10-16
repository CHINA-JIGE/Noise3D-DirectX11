
/***********************************************************************

								¿‡£∫NOISE GUI manager

				Description : manage different GUI objects

************************************************************************/

#include "Noise3D.h"

NoiseGUIManager::NoiseGUIManager()
{
	m_pChildGraphicObject = new NoiseGraphicObject;
	m_pChildButtonList	= new std::vector<NoiseGUIButton*>;
	m_pChildLabelList		= new std::vector<NoiseGUILabel*>;
	m_pEventsList			= new std::vector<NOISE_GUI_EVENTS>;
};

void NoiseGUIManager::AddToRenderList()
{

	if (m_pFatherScene->m_pChildRenderer)
	{
		//the GUI holds a graphic Object to store graphic entity of GUI responsive object
		m_pFatherScene->m_pChildRenderer->m_pRenderList_GraphicObject->push_back(m_pChildGraphicObject);
	}
};

BOOL NoiseGUIManager::AddButton(NoiseGUIButton * pButton)
{
	//add button to child button list
	if (pButton)
	{
		m_pChildButtonList->push_back(pButton);
		pButton->m_pFatherGUIMgr = this;
	}
	else
	{
		DEBUG_MSG1("Noise GUI mgr : Add Button : Ptr invalid!");
		return TRUE;
	}
	return TRUE;
}

BOOL NoiseGUIManager::AddLabel(NoiseGUILabel * pLabel)
{
	//add label ( pure text)
	if (pLabel)
	{
		m_pChildLabelList->push_back(pLabel);
		pLabel->m_pFatherGUIMgr = this;
	}
	else
	{
		DEBUG_MSG1("Noise GUI mgr : Add Label : Ptr invalid!");
		return TRUE;
	}

	return TRUE;
}

void NoiseGUIManager::Update()
{
}

void NoiseGUIManager::GetMessage(std::vector<NOISE_GUI_EVENTS>& eventsList)
{

};







