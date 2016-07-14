
/***********************************************************************

                           类：NOISE Engine

			简述：主要负责管理全局接口和变量，初始化等

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

static IRoot* static_pEngine;

//构造函数
IRoot::IRoot()
{
	mRenderWindowTitle = L"Noise 3D - Render Window";
	m_pMainLoopFunction = nullptr;
	static_pEngine = this;
}

IRoot::~IRoot()
{
	m_pMainLoopFunction = nullptr;
	ReleaseAll();
}

HWND IRoot::CreateRenderWindow(UINT pixelWidth, UINT pixelHeight, LPCWSTR windowTitle, HINSTANCE hInstance)
{

	WNDCLASS wndclass;
	HWND outHWND;//句柄
	mRenderWindowHINSTANCE = hInstance;
	mRenderWindowTitle = windowTitle;
	mRenderWindowClassName = L"Noise Engine Render Window";

	//窗体类注册
	if (mFunction_InitWindowClass(&wndclass) == FALSE)
	{
		DEBUG_MSG1("Window Class 创建失败");
		return FALSE;
	};

	//创建窗体

	outHWND = mFunction_InitWindow();
	if (outHWND == 0)
	{
		DEBUG_MSG1("窗体创建失败");
		return FALSE;
	};

	return outHWND;
};

BOOL IRoot::InitD3D(HWND RenderHWND, UINT BufferWidth, UINT BufferHeight, BOOL IsWindowed)
{
	mRenderWindowHWND = RenderHWND;
	gMainBufferPixelWidth = BufferWidth;
	gMainBufferPixelHeight = BufferHeight;

	HRESULT hr = S_OK;
#pragma region InitDevice11
	//用来做判断及返回结果

	//硬件驱动类型
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,	//HAL 硬件驱动
		D3D_DRIVER_TYPE_REFERENCE,	//REF参考设备
		D3D_DRIVER_TYPE_WARP,		//Windows Advanced Rasterization Platform只支持DX10.1
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	//D3D特性的版本
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//设备创建标签 
	UINT createDeviceFlags = 0;
#if defined(DEBUG)||defined(_DEBUG)		//D3D调试模式
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


	//用列举出来的硬件方式 尝试初始化 直到成功
	UINT driverTypeIndex = 0;
	for (driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		//D3D_DRIVER_TYPE 
		g_Device_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(
			NULL,				//null表示使用主显示器
			g_Device_driverType,		//驱动类型 HAL/REF
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,//createDeviceFlags,	//是不是调试模式	
			featureLevels,		//让D3D选择的特性的版本
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&g_pd3dDevice11,		//返回D3D设备指针
			&g_Device_featureLevel,	//返回最终使用的特性的版本
			&g_pImmediateContext//返回
			);
		//创建成功了就不用继续尝试创建
		if (SUCCEEDED(hr))
		{
			break;
		};
	};
	//尝试创建设备失败
	HR_DEBUG(hr, "d3d设备创建失败");


	//检测多重采样
	g_pd3dDevice11->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &g_Device_MSAA4xQuality);//4x坑锯齿一般都支持，这个返回值一般情况下都大于0
	if (g_Device_MSAA4xQuality > 0)
	{
		g_Device_MSAA4xEnabled = TRUE;	//4x抗锯齿可以开了
	};

	//ReleaseCOM(g_pd3dDevice11);


	/*填充交换链的属性
	交换链，用于管理BUFEER的交换，主要处理back与front
	可以用于多窗口渲染
	DESC = Description*/
	DXGI_SWAP_CHAIN_DESC SwapChainParam;
	ZeroMemory(&SwapChainParam, sizeof(SwapChainParam));
	SwapChainParam.BufferCount = 1;
	SwapChainParam.BufferDesc.Width = gMainBufferPixelWidth;
	SwapChainParam.BufferDesc.Height = gMainBufferPixelHeight;
	SwapChainParam.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainParam.BufferDesc.RefreshRate.Numerator = 60;//	分子= =？
	SwapChainParam.BufferDesc.RefreshRate.Denominator = 1;//分母
	SwapChainParam.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//BACKBUFFER怎么被使用
	SwapChainParam.OutputWindow = RenderHWND;
	SwapChainParam.Windowed = IsWindowed;
	SwapChainParam.SampleDesc.Count = (g_Device_MSAA4xEnabled = TRUE ? 4 : 1);//多重采样倍数
	SwapChainParam.SampleDesc.Quality = (g_Device_MSAA4xEnabled = TRUE ? g_Device_MSAA4xQuality - 1 : 0);//quality之前获取了

	 //下面的COM的QueryInterface 用一个接口查询另一个接口
	IDXGIDevice *dxgiDevice = 0;
	g_pd3dDevice11->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter *dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory *dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	
	//终于创建了一个交换链
	hr = dxgiFactory->CreateSwapChain(
		g_pd3dDevice11,		//设备的指针
		&SwapChainParam,	//交换链的描述
		&g_pSwapChain);		//返回的交换链指针
	HR_DEBUG(hr, "SwapChain创建失败！");

	dxgiFactory->Release();
	dxgiDevice->Release();
	dxgiAdapter->Release();
#pragma endregion InitDevice11

	//创建缓冲区和渲染视口，深度/模版 视口
	//这些Views是用来绑定到pipeline上
#pragma region CreateViews

	// 创建一个(可以多个)渲染视图RENDER TARGET VIEW
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return FALSE;

	hr = g_pd3dDevice11->CreateRenderTargetView(
		pBackBuffer,
		NULL,					//可以填充一个D3D11_RENDERTARGETVIEW_DESC
		&g_pRenderTargetView);	//返回一个渲染视口

	pBackBuffer->Release();		//已经用完了的临时接口- -

	//ReleaseCOM(g_pd3dDevice11);

	HR_DEBUG(hr, "创建RENDER TARGET VIEW失败");



	//创建depth/stencil view
	D3D11_TEXTURE2D_DESC DSBufferDesc;
	DSBufferDesc.Width = BufferWidth;
	DSBufferDesc.Height = BufferHeight;
	DSBufferDesc.MipLevels = 1;
	DSBufferDesc.ArraySize = 1;
	DSBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSBufferDesc.SampleDesc.Count = (g_Device_MSAA4xEnabled = TRUE ? 4 : 1);
	DSBufferDesc.SampleDesc.Quality = (g_Device_MSAA4xEnabled = TRUE ? g_Device_MSAA4xQuality - 1 : 0);
	DSBufferDesc.Usage = D3D11_USAGE_DEFAULT;	//尽量避免DYNAMIC和STAGING
	DSBufferDesc.CPUAccessFlags = 0;	//CPU不能碰它 GPU才行 这样能够加快
	DSBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;//和PIPELINE的绑定
	DSBufferDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthStencilBuffer;
	g_pd3dDevice11->CreateTexture2D(&DSBufferDesc, 0, &pDepthStencilBuffer);//创建一个缓冲区
	hr = g_pd3dDevice11->CreateDepthStencilView(
		pDepthStencilBuffer,
		0,
		&g_pDepthStencilView);	//返回一个depth/stencil视口指针

	//ReleaseCOM(g_pd3dDevice11);
	pDepthStencilBuffer->Release();

	if (FAILED(hr))
	{
		return FALSE;
	};


	//设置渲染对象：刚刚创建的渲染视口和depth/stencil的
	//这就是绑定到pipeline
	g_pImmediateContext->OMSetRenderTargets(
		1,
		&g_pRenderTargetView,
		g_pDepthStencilView);

#pragma endregion CreateViews


	//XY都是-1到1，深度Z是0到1，DX11不会默认创建视口，DX9就会
#pragma region CreateViewPort

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)BufferWidth;		//视口WIDTH 跟后缓冲区一样
	vp.Height = (FLOAT)BufferHeight;	//视口Height
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	//SetViewport 参数1：视口的个数 参数2：视口数组的首地址
	g_pImmediateContext->RSSetViewports(1, &vp);

#pragma endregion CreateViewPort

	//ReleaseCOM(g_pd3dDevice11);
	return TRUE;

};

void IRoot::ReleaseAll()//考虑下在构造函数那弄个AddToReleaseList呗
{
	//g_pImmediateContext->Flush();
	//g_pImmediateContext->ClearState();

	m_pS.DeleteObject();

	ReleaseCOM(g_pRenderTargetView);
	ReleaseCOM(g_pSwapChain);
	ReleaseCOM(g_pVertexLayout_Default);
	ReleaseCOM(g_pVertexLayout_Simple);
	ReleaseCOM(g_pDepthStencilView);
	ReleaseCOM(g_pImmediateContext);
	//check live object
#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug *d3dDebug;
	HRESULT hr = g_pd3dDevice11->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	if (d3dDebug != nullptr)			d3dDebug->Release();
#endif


	ReleaseCOM(g_pd3dDevice11);	

}

void IRoot::Mainloop()
{
	MSG msg;//消息体
	ZeroMemory(&msg, sizeof(msg));

	//在程序还没有接收到“退出”消息的时候
	while (msg.message != WM_QUIT)
	{
		/*PM_REMOVE  PeekMessage处理后，消息从队列里除掉。
		而函数PeekMesssge是以查看的方式从系统中获取消息，
		可以不将消息从系统中移除，是非阻塞函数；
		当系统无消息时，返回FALSE，继续执行后续代码。*/

		//有消息的时候赶紧处理了 即Peek返回TRUE的时候
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);//翻译消息并发送到windows消息队列
			DispatchMessage(&msg);//接收信息
		}
		else
			//消息队列没东西处理了 那就搞主循环
		{
			switch (mMainLoopStatus)
			{
			case NOISE_MAINLOOP_STATUS_BUSY:
				//如果主循环函数有效
				if (m_pMainLoopFunction)
				{
					(*m_pMainLoopFunction)();
				}
					break;

			case NOISE_MAINLOOP_STATUS_QUIT_LOOP:
				return;
				break;

			default:
				break;

			}//switch
		}
	};

	//ReleaseAll();
	UnregisterClass(mRenderWindowClassName, mRenderWindowHINSTANCE);
}

void IRoot::SetMainLoopFunction( void (*pFunction)(void) )
{
	m_pMainLoopFunction =pFunction;
}

void IRoot::SetMainLoopStatus(NOISE_MAINLOOP_STATUS loopStatus)
{
	mMainLoopStatus = loopStatus;
}

HWND IRoot::GetRenderWindowHWND()
{
	return mRenderWindowHWND;
}

HINSTANCE IRoot::GetRenderWindowHINSTANCE()
{
	return mRenderWindowHINSTANCE;
}

int	IRoot::GetRenderWindowWidth()
{
	RECT windowRect;
	//when you only need the difference , GetClientRect is OK~
	GetClientRect(mRenderWindowHWND, &windowRect);
	return (int)(windowRect.right- windowRect.left);
}

int	IRoot::GetRenderWindowHeight()
{
	RECT windowRect;
	GetClientRect(mRenderWindowHWND, &windowRect);
	return (int)(windowRect.bottom - windowRect.top);
};



/************************************************************************
										 PRIVATE                               
************************************************************************/

//windows message handle
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)//消息的处理程序
{
	/*HDC			hdc ;
	PAINTSTRUCT ps ;
	RECT        rect ;*/
	switch (message)
	{
	case WM_CREATE:
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);

}

BOOL IRoot::mFunction_InitWindowClass(WNDCLASS* wc)
{
	wc->style = CS_HREDRAW | CS_VREDRAW; //样式
	wc->cbClsExtra = 0;
	wc->cbWndExtra = 0;
	wc->hInstance = mRenderWindowHINSTANCE;//窗体实例名，由windows自动分发
	wc->hIcon = LoadIcon(NULL, IDI_APPLICATION);//显示上面的图标titlte
	wc->hCursor = LoadCursor(NULL, IDC_ARROW);//窗口光标
	wc->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//背景刷
	wc->lpszMenuName = NULL;
	wc->lpfnWndProc = WndProc;//设置窗体接收windws消息函数
	wc->lpszClassName = mRenderWindowClassName;//窗体类名

	if (!RegisterClass(wc))//注册窗体类
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), TEXT("R"), MB_ICONERROR);
		return FALSE;
	}
	else
	{
		return TRUE;
	};

};

HWND IRoot::mFunction_InitWindow()
{
	UINT scrWidth = GetSystemMetrics(SM_CXSCREEN);
	UINT scrHeight = GetSystemMetrics(SM_CYSCREEN);

	HWND hwnd;
	hwnd = CreateWindow(
		mRenderWindowClassName,      // window class name
		mRenderWindowTitle,   // window caption
		WS_OVERLAPPEDWINDOW, // window style ------WS_OVERLAPPEDWINDOW/WS_POPUP
		//WS_POPUP, // window style ------WS_OVERLAPPEDWINDOW/WS_POPUP
		scrWidth/6,// initial x position ---------CW_USEDEFAULT
		scrHeight/6,// initial y position
		640,// initial x size
		480,// initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		mRenderWindowHINSTANCE, // program instance handle
		NULL);

	if (hwnd == 0)
	{
		//创建失败
		return 0;
	}
	else
	{
		//创建成功
		ShowWindow(hwnd, SW_RESTORE);//显示窗口
		UpdateWindow(hwnd);//更新窗体
		return hwnd;
	};

};
