
/***********************************************************************

				CLASS: GUI Events Common Operation

************************************************************************/
#pragma once

namespace Noise3D
{
	namespace GUI
	{
		struct N_GUI_MOUSE_INFO
		{
			BOOL isMouseLeftPressedDown;
			BOOL isMouseMoving;
			float mouseX_backBufferSpace;
			float mouseY_backBufferSpace;
			UINT mouseDepthLevel;
			float mouseScrollDiff;
		};


		//a base class that provide general life cycle management
		class _declspec(dllexport) IComponentBase :
			public IBasicContainerInfo
		{
			friend class IComponentManager;

		public:

			IComponentBase();

			//passive msg acquisition
			void	SetEventProcessCallbackFunction(UINT(*pFunc)(UINT NoiseGUIEvent));

			//active event acquisition
			void	GetEventList(std::vector<UINT>& outEventList);

			//是否已获取焦点
			BOOL IsFocused();

			void	SetDepthLevel(UINT depth);

			UINT GetDepthLevel();

		protected:

			//update events /callback message process function
			void	mFunction_CallMsgProcFunctionAndUpdateEventList(std::vector<UINT>& eventList);

			//1.validate focus
			//2.mouse event:button down/up/move...
			//the POS Z is used to implement component overlapping
			void	mFunction_GenerateMouseEventsAndValidateFocus(std::vector<UINT>& outEventList, BOOL isMouseInContainer, N_GUI_MOUSE_INFO& mouseInfo);

			UINT		mDepthLevel;//determine the order of overlapping ; MAX: NOISE_GUI_MAX_POS_Z
			BOOL	mHasBeenPressedDown;
			BOOL	mIsFocused;
			UINT(*m_pFunction_EventMessageProcess)(UINT NoiseGUIEvent);//a callback function pointer
			std::vector<UINT>*	m_pEventList;//Events message for this button
		};
	}
}