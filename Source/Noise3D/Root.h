
/***********************************************************************

										h£ºRoot

************************************************************************/

#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ IRoot 
	: private IFileManager, public IFactory<IScene>
	{
	public:

		IScene*	GetScenePtr();

		HWND	CreateRenderWindow(UINT pixelWidth, UINT pixelHeight, LPCWSTR windowTitle, HINSTANCE hInstance);

		BOOL	InitD3D(HWND RenderHWND);

		void	ReleaseAll();

		void Mainloop();

		void	SetMainLoopFunction(void(*pFunction)(void));//function pointer

		void	SetMainLoopStatus(NOISE_MAINLOOP_STATUS loopStatus);

		HWND GetRenderWindowHWND();

		HINSTANCE GetRenderWindowHINSTANCE();

		int	 GetRenderWindowWidth();

		int	 GetRenderWindowHeight();

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
		HWND				mRenderWindowHWND;

		//function pointer used to encapsule the "Main Loop" into the framwork
		void(*m_pMainLoopFunction)(void);
		NOISE_MAINLOOP_STATUS		mMainLoopStatus;

	private:

		//invoked by RenderWindows Creation function
		BOOL	mFunction_InitWindowClass(WNDCLASS* wc);

		//invoked by RenderWindows Creation function
		HWND mFunction_InitWindow(UINT windowWidth, UINT windowHeight);

		//Create Effect Interface From fxo file
		BOOL	mFunction_CreateEffectFromMemory();
	};


	extern /*_declspec(dllexport)*/ IRoot* GetRoot();


}
