
/***********************************************************************

										h：NoiseEngine

************************************************************************/

#pragma once


class _declspec(dllexport) NoiseEngine:public NoiseClassLifeCycle
{
public:

	//构造函数
	NoiseEngine();

	HWND	CreateRenderWindow(UINT pixelWidth,UINT pixelHeight,LPCWSTR windowTitle, HINSTANCE hInstance);

	BOOL	InitD3D(HWND RenderHWND,UINT BufferWidth,UINT BufferHeight,BOOL IsWindowed);

	void	ReleaseAll();

	void Mainloop();

	void	SetMainLoopFunction( void (*pFunction)(void));//function pointer

	void	SetMainLoopStatus(NOISE_MAINLOOP_STATUS loopStatus);

	HWND GetRenderWindowHWND();

	HINSTANCE GetRenderWindowHINSTANCE();

	int	 GetRenderWindowWidth();

	int	 GetRenderWindowHeight();

private:

	void		Destroy();

	//this is for built-in window creation function
	LPCWSTR			mRenderWindowTitle;
	LPCWSTR			mRenderWindowClassName;
	HINSTANCE		mRenderWindowHINSTANCE;
	HWND				mRenderWindowHWND;

	UINT					mRenderWindowPixelWidth;
	UINT					mRenderWindowPixelHeight;

	//用于储存主循环的函数指针 因为要封装消息循环 所以只能这样了
	 void				(*m_pMainLoopFunction)(void);
	 NOISE_MAINLOOP_STATUS		mMainLoopStatus;

private:

	 //创建渲染窗口的子函数
	BOOL	mFunction_InitWindowClass(WNDCLASS* wc);
	//创建渲染窗口的子函数
	HWND mFunction_InitWindow();
};

