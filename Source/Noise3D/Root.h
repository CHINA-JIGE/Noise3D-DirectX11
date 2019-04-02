
/***********************************************************************

										h£ºRoot

************************************************************************/

#pragma once

namespace Noise3D
{
	enum NOISE_MAINLOOP_STATUS
	{
		NOISE_MAINLOOP_STATUS_RUNNING = 0,
		NOISE_MAINLOOP_STATUS_QUIT_LOOP = 1,
	};

	class IMainloop
	{
	public:
		virtual void Callback_Mainloop() = 0;
	};


	class /*_declspec(dllexport)*/ Root :
		private IFileIO, 
		protected IFactory<SceneManager>
	{
	public:

		SceneManager*	GetSceneMgrPtr();

		HWND		CreateRenderWindow(UINT pixelWidth, UINT pixelHeight, LPCWSTR windowTitle, HINSTANCE hInstance);

		bool		Init();

		//void		ReleaseAll();

		//mainloop body, IMainloop interface or function ptr is called back.
		//it depends on which one is valid.
		//if both of them are valid, then IMainloop* interface will be PRIORITIZED.
		void		Mainloop();

		//function pointer, i want to deprecate it one day. only static func can be passed in
		void		SetMainloopFunction(void(*pFunction)(void));

		//call-back interface, has most priority
		void		SetMainloopFunction(IMainloop* pInterface);

		void		SetMainLoopStatus(NOISE_MAINLOOP_STATUS loopStatus);

	private:

		friend IFactory<Root>;

		//constructor
		Root();

		//destructor
		~Root();

		UINT mMainBackBufferWidth;
		UINT mMainBackBufferHeight;

		//this is for built-in window creation function
		LPCWSTR			mRenderWindowTitle;
		LPCWSTR			mRenderWindowClassName;
		HINSTANCE		mRenderWindowHINSTANCE;
		//HWND				mRenderWindowHWND;

		//function pointer used to encapsule the "Main Loop" into the framwork
		void(*m_pMainLoopFunction)(void);//called by Mainloop
		IMainloop* m_pMainloopInterface;//called by MainloopEx
		NOISE_MAINLOOP_STATUS	mMainLoopStatus;

	private:

		//invoked by RenderWindows Creation function
		bool	mFunction_InitWindowClass(WNDCLASS* wc);

		//invoked by RenderWindows Creation function
		HWND mFunction_InitWindow(UINT windowWidth, UINT windowHeight);

		//Create Effect Interface From fxo file
		bool	mFunction_CreateEffectFromMemory();
	};


	extern /*_declspec(dllexport)*/ Root* GetRoot();


}
