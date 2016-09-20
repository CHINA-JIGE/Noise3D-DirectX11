
/***********************************************************************

										h：Root

************************************************************************/

#pragma once

namespace Noise3D
{

	class /*_declspec(dllexport)*/ IRoot
	:public IFactory<IScene>
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

		//创建渲染窗口的子函数
		BOOL	mFunction_InitWindowClass(WNDCLASS* wc);
		//创建渲染窗口的子函数
		HWND mFunction_InitWindow(UINT windowWidth, UINT windowHeight);
	};


	extern /*_declspec(dllexport)*/ IRoot* GetRoot();


}
