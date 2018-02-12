
/***********************************************************************

                           类：NOISE Engine

			简述：主要负责管理全局接口和变量，初始化等

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

/*_declspec(dllexport)*/ IRoot* Noise3D::GetRoot()
{
	class IRootCreation :public IFactory<IRoot>
	{
	public:

		IRootCreation() :IFactory<IRoot>(1) {};

		~IRootCreation() {}

		IRoot* GetRoot()
		{
			static int rootCount = 0;
			//if a Root was never created, create one
			if (rootCount == 0)
			{
				rootCount++;
				m_pRoot = IFactory<IRoot>::CreateObject("Root");
				return m_pRoot;
			}
			else
			{
				//return the existed IRoot
				return m_pRoot;
			}

		};

	private:
		friend class IFactory<IRoot>;

		IRoot* m_pRoot;
	};

	static IRootCreation rootCreationFactory;
	static IRoot* ptr = rootCreationFactory.GetRoot();
	return  ptr;
};



//Constructor
IRoot::IRoot() :
	IFactory<IScene>(1)
{
	mRenderWindowTitle = L"Noise 3D - Render Window";
	m_pMainLoopFunction = nullptr;
	mMainLoopStatus = NOISE_MAINLOOP_STATUS_BUSY;
	mMainBackBufferHeight = 0;
	mMainBackBufferWidth = 0;
}

IRoot::~IRoot()
{
	m_pMainLoopFunction = nullptr;
	ReleaseAll();
}

IScene* IRoot::GetScenePtr()
{
	const N_UID sceneUID = "myScene";
	//first time to get a ScenePtr,Create one
	if (IFactory<IScene>::GetObjectCount() == 0)
	{
		IFactory<IScene>::CreateObject(sceneUID);
	}
	return IFactory<IScene>::GetObjectPtr(sceneUID);
}

HWND IRoot::CreateRenderWindow(UINT pixelWidth, UINT pixelHeight, LPCWSTR windowTitle, HINSTANCE hInstance)
{

	WNDCLASS wndclass;
	HWND outHWND;//句柄
	mRenderWindowHINSTANCE = hInstance;
	mRenderWindowTitle = windowTitle;
	mRenderWindowClassName = L"Noise Engine Render Window";

	//窗体类注册
	if (mFunction_InitWindowClass(&wndclass) == false)
	{
		ERROR_MSG("Window Class 创建失败");
		return false;
	};

	//创建窗体

	outHWND = mFunction_InitWindow(pixelWidth,pixelHeight);
	if (outHWND == 0)
	{
		ERROR_MSG("窗体创建失败");
		return false;
	};

	return outHWND;
};

bool IRoot::Init()
{
	HRESULT hr = S_OK;

	//硬件驱动类型
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,	//HAL
		D3D_DRIVER_TYPE_REFERENCE,	//REF device
		D3D_DRIVER_TYPE_WARP,		//Windows Advanced Rasterization Platform, D3D10.1 only
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	//D3D feature levels (9_1,9_29_3, 10_0 ,10_1,11_0,11_1etc.)
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//flag of device creation:
	UINT createDeviceFlags = 0;
#ifdef _DEBUG	
	//device debug mode, enable outputing d3d info
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
			createDeviceFlags,//createDeviceFlags,	//是不是调试模式	
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
	HR_DEBUG(hr, "IRoot : D3D Device Creation failed /n DriverType"+std::to_string(g_Device_driverType));

	
	if(!mFunction_CreateEffectFromMemory())return false;

	return true;

};

void IRoot::ReleaseAll()
{

	IScene* pScene = GetScenePtr();
	pScene->ReleaseAllChildObject();
	IFactory<IScene>::DestroyAllObject();//delete the only scene

	ReleaseCOM(g_pVertexLayout_Default);
	ReleaseCOM(g_pVertexLayout_Simple);
	ReleaseCOM(g_pImmediateContext);
	ReleaseCOM(g_pFX);
	//check live object
#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug *d3dDebug;
	HRESULT hr = S_OK;

	if (g_pd3dDevice11 != nullptr)
	{
		hr = g_pd3dDevice11->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
		if (SUCCEEDED(hr))
		{
			hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}

		ReleaseCOM(d3dDebug);
		ReleaseCOM(g_pd3dDevice11);
	}

#endif
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


/************************************************************************
										 PRIVATE                               
************************************************************************/

//windows message handle
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

bool IRoot::mFunction_InitWindowClass(WNDCLASS* wc)
{
	wc->style = CS_HREDRAW | CS_VREDRAW; //样式
	wc->cbClsExtra = 0;
	wc->cbWndExtra = 0;
	wc->hInstance = mRenderWindowHINSTANCE;//窗体实例名，由windows自动分发
	wc->hIcon = LoadIcon(NULL, IDI_APPLICATION);//显示上面的图标titlte
	wc->hCursor = LoadCursor(NULL, IDC_ARROW);//窗口光标
	wc->hbrBackground = NULL;// (HBRUSH)GetStockObject(WHITE_BRUSH);//背景刷
	wc->lpszMenuName = NULL;
	wc->lpfnWndProc = WndProc;//设置窗体接收windws消息函数
	wc->lpszClassName = mRenderWindowClassName;//窗体类名

	if (!RegisterClass(wc))//注册窗体类
	{
		ERROR_MSG("IRoot: Create Window failed! Windows NT (or newer) required");
		return false;
	}
	else
	{
		return true;
	};

};

HWND IRoot::mFunction_InitWindow(UINT windowWidth,UINT windowHeight)
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
		windowWidth,// initial x size
		windowHeight,// initial y size
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

bool	IRoot::mFunction_CreateEffectFromMemory()
{
	std::vector<char> compiledShader;
	HRESULT hr = S_OK;

	//Load FXO file
#ifdef _DEBUG
	if (!IFileIO::ImportFile_PURE("shader//Main_d.fxo", compiledShader))
	{
		ERROR_MSG("Root : critical error! Compiled shader (Debug mode) not found!!");
		return false;
	}
#else
	if (!IFileIO::ImportFile_PURE("shader//Main.fxo", compiledShader))
	{
		ERROR_MSG("Root : critical error! Compiled shader(Release mode) not found!!");
		return false;
	}
#endif

	//Create Effect Framework
	hr = D3DX11CreateEffectFromMemory(&compiledShader.at(0), compiledShader.size(), 0, g_pd3dDevice11, &g_pFX);
//#else

	//N_InternalResourceInfo shaderInResource = Noise3D::GetInternalResource(NOISE_INTERNAL_RESOURCE_MAINSHADER);
	//Create Effect Framework
	//hr = D3DX11CreateEffectFromMemory(shaderInResource.pBuff, shaderInResource.size(), 0, g_pd3dDevice11, &g_pFX);

//#endif


	HR_DEBUG(hr, "IRoot : load compiled shader failed");

	return true;
}


