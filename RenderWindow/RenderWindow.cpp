#include "RenderWindow.h"

//using namespace Noise3D;
//NoiseEngine NsEngine;


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	MSG msg;//消息体
	ZeroMemory(&msg,sizeof(msg));
	WNDCLASS wndclass;
	HWND hwnd;//句柄

	//窗体类注册
	if(InitWindowClass(&wndclass,hInstance) == FALSE)
	{
		MessageBox(0,"窗体类创建失败","FAIL",0);
		return 0;
	};

	//hwnd边赋值边判断别写在if里面了= =
	hwnd = InitWindow(hInstance);
	if(hwnd  == 0)
	{
		MessageBox(0,"窗体创建失败","FAIL",0);
		return 0;
	};

	//初始化3D引擎
	if(Init3D(hwnd) == FALSE)
	{
		MessageBox(0,"D3D初始化失败","FAIL",0);
		return 0;
	};

	//在程序还没有接收到“退出”消息的时候
	while(msg.message != WM_QUIT)
	{
		/*PM_REMOVE  PeekMessage处理后，消息从队列里除掉。
		而函数PeekMesssge是以查看的方式从系统中获取消息，
		可以不将消息从系统中移除，是非阻塞函数；
		当系统无消息时，返回FALSE，继续执行后续代码。*/

		//有消息的时候赶紧处理了 即Peek返回TRUE的时候
		if(PeekMessage(&msg,NULL,0U,0U,PM_REMOVE))
		{
			TranslateMessage (&msg);//翻译消息并发送到windows消息队列
			DispatchMessage (&msg) ;//接收信息
		}
		else
			//消息队列没东西处理了 那就搞主循环
		{
			MainLoop();
		};
	}
	Cleanup();//清理
	UnregisterClass(WindowClassName, hInstance );
	return msg.wParam ;
}



BOOL InitWindowClass(WNDCLASS* wc,HINSTANCE hInstance)
{
	wc->style = CS_HREDRAW | CS_VREDRAW ; //样式
	wc->cbClsExtra   = 0 ;
	wc->cbWndExtra   = 0 ;
	wc->hInstance = hInstance ;//窗体实例名，由windows自动分发
	wc->hIcon = LoadIcon (NULL, IDI_APPLICATION) ;//显示上面的图标titlte
	wc->hCursor = LoadCursor (NULL, IDC_ARROW) ;//窗口光标
	wc->hbrBackground= (HBRUSH) GetStockObject (WHITE_BRUSH) ;//背景刷
	wc->lpszMenuName=NULL;
	wc->lpfnWndProc=WndProc;//设置窗体接收windws消息函数
	wc->lpszClassName= WindowClassName;//窗体类名

	if (!RegisterClass (wc))//注册窗体类
	{
		MessageBox ( NULL, TEXT ("This program requires Windows NT!"), TEXT("R"), MB_ICONERROR) ;
		return FALSE ;
	}
	else
	{
		return TRUE;
	};

};

HWND InitWindow(HINSTANCE hInstance)
{
	HWND hwnd;
	hwnd = CreateWindow(WindowClassName,      // window class name
		AppName,   // window caption
		WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT,// initial x position
		CW_USEDEFAULT,// initial y position
		640,// initial x size
		480,// initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL) ;

	if (hwnd == 0)
	{
		//创建失败
		return 0;
	}
	else
	{
		//创建成功
		ShowWindow (hwnd,SW_RESTORE);//显示窗口
		UpdateWindow (hwnd) ;//更新窗体
		return hwnd;
	};

};

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)//消息的处理程序
{
	/*HDC			hdc ;
	PAINTSTRUCT ps ;
	RECT        rect ;*/
	switch (message)
	{
	case WM_CREATE:
		return 0 ;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;

}


