
/***********************************************************************

										h£ºRoot

************************************************************************/

#pragma once

namespace Noise3D
{
	enum NOISE_MAINLOOP_STATUS
	{
		NOISE_MAINLOOP_STATUS_BUSY = 0,
		NOISE_MAINLOOP_STATUS_QUIT_LOOP = 1,
	};


	class /*_declspec(dllexport)*/ IRoot 
	: private IFileIO, public IFactory<IScene>
	{
	public:

		IScene*	GetScenePtr();

		HWND		CreateRenderWindow(UINT pixelWidth, UINT pixelHeight, LPCWSTR windowTitle, HINSTANCE hInstance);

		bool		Init();

		void		ReleaseAll();

		void		Mainloop();

		void		SetMainLoopFunction(void(*pFunction)(void));//function pointer

		void		SetMainLoopStatus(NOISE_MAINLOOP_STATUS loopStatus);

	private:

		friend IFactory<IRoot>;

		//constructor
		IRoot();

		//destructor
		~IRoot();

		UINT mMainBackBufferWidth;
		UINT mMainBackBufferHeight;

		//this is for built-in window creation function
		LPCWSTR			mRenderWindowTitle;
		LPCWSTR			mRenderWindowClassName;
		HINSTANCE		mRenderWindowHINSTANCE;
		//HWND				mRenderWindowHWND;

		//function pointer used to encapsule the "Main Loop" into the framwork
		void(*m_pMainLoopFunction)(void);
		NOISE_MAINLOOP_STATUS		mMainLoopStatus;

	private:

		//invoked by RenderWindows Creation function
		bool	mFunction_InitWindowClass(WNDCLASS* wc);

		//invoked by RenderWindows Creation function
		HWND mFunction_InitWindow(UINT windowWidth, UINT windowHeight);

		//Create Effect Interface From fxo file
		bool	mFunction_CreateEffectFromMemory();
	};


	extern /*_declspec(dllexport)*/ IRoot* GetRoot();


}
