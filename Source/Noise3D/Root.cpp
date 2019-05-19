
/***********************************************************************

									class：Root

			desc: the first class that should be created when 
			using Noise3D. handle some basic global var.

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

/*_declspec(dllexport)*/ Root* Noise3D::GetRoot()
{
	class IRootCreation :public IFactory<Root>
	{
	public:

		IRootCreation() :IFactory<Root>(1) {};

		~IRootCreation() {}

		Root* GetRoot()
		{
			static int rootCount = 0;
			//if a Root was never created, create one
			if (rootCount == 0)
			{
				rootCount++;
				m_pRoot = IFactory<Root>::CreateObject("Root");
				return m_pRoot;
			}
			else
			{
				//return the existed Root
				return m_pRoot;
			}

		};

	private:
		friend class IFactory<Root>;

		Root* m_pRoot;
	};

	static IRootCreation rootCreationFactory;
	static Root* ptr = rootCreationFactory.GetRoot();
	return  ptr;
};

//Constructor
Root::Root() :
	IFactory<SceneManager>(1),
	m_pMainloopInterface(nullptr),
	m_pMainLoopFunction(nullptr),
	mMainLoopStatus(NOISE_MAINLOOP_STATUS_RUNNING),
	mRenderWindowClassName(nullptr),
	mRenderWindowHINSTANCE(NULL),
	mMainBackBufferWidth(0),
	mMainBackBufferHeight(0)
{
	mRenderWindowTitle = L"Noise 3D - Render Window";
}

Root::~Root()
{
	m_pMainLoopFunction = nullptr;
	m_pMainloopInterface = nullptr;
	mMainLoopStatus = NOISE_MAINLOOP_STATUS_QUIT_LOOP;

	if (mRenderWindowClassName)
	{
		UnregisterClass(mRenderWindowClassName, mRenderWindowHINSTANCE);
		mRenderWindowClassName = NULL;
	}

	SceneManager* pScene = GetSceneMgrPtr();
	pScene->ReleaseAllChildObject();
	//IFactory<SceneManager>::DestroyAllObject();

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

SceneManager* Root::GetSceneMgrPtr()
{
	const N_UID sceneUID = "myScene";
	//first time to get a ScenePtr,Create one
	if (IFactory<SceneManager>::GetObjectCount() == 0)
	{
		IFactory<SceneManager>::CreateObject(sceneUID);
	}
	return IFactory<SceneManager>::GetObjectPtr(sceneUID);
}

HWND Root::CreateRenderWindow(UINT pixelWidth, UINT pixelHeight, LPCWSTR windowTitle, HINSTANCE hInstance)
{

	WNDCLASS wndclass;
	HWND outHWND;//handle
	mRenderWindowHINSTANCE = hInstance;
	mRenderWindowTitle = windowTitle;
	mRenderWindowClassName = L"Noise Engine Render Window";

	//register window class
	if (mFunction_InitWindowClass(&wndclass) == false)
	{
		ERROR_MSG("Root: Failed to create window class.");
		return false;
	};

	outHWND = mFunction_InitWindow(pixelWidth,pixelHeight);
	if (outHWND == 0)
	{
		ERROR_MSG("Root: Failed to create render window.");
		return false;
	};

	return outHWND;
};

bool Root::Init()
{
	HRESULT hr = S_OK;

	//hard-driven types
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
	//(2018.7.20)note that in Win10 SDK, 'Graphics Tools' must be installed to enable D3D11_CREATE_DEVICE_DEBUG
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
	HR_DEBUG(hr, "Root : D3D Device Creation failed /n DriverType"+std::to_string(g_Device_driverType));

	
	if(!mFunction_CreateEffectFromMemory())return false;

	return true;

};

void Root::Mainloop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		/*
		PM_REMOVE : after PeekMessage(), remove to msg from queue.
		PeekMessage() just 'Peek' message from system in a
		NON-BLOCKED way(thus it can't remove message from the queue)
		when no message sent from system, return FALSE, 
		then continue to execute the following program.
		*/

		//handles the message if returns TRUE
		//(it's necessary, or WM_RESIZE, WM_PAINT or other system msg will be ignore
		//then the window will have no respondance)
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				if (m_pMainloopInterface)m_pMainloopInterface->Callback_Cleanup();
				return;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else//no message to handle, we'll call the main loop
		{
			switch (mMainLoopStatus)
			{
			case NOISE_MAINLOOP_STATUS_RUNNING:
				if (m_pMainloopInterface)
				{
					m_pMainloopInterface->Callback_Mainloop();
				}
				else if (m_pMainLoopFunction)
				{
					(*m_pMainLoopFunction)();
				}
				break;

			case NOISE_MAINLOOP_STATUS_QUIT_LOOP:
				if (m_pMainloopInterface)
				{
					m_pMainloopInterface->Callback_Cleanup();
				}
				return;
				break;

			default:
				break;

			}//switch
		}
	};

}

void Root::SetMainloopFunction( void (*pFunction)(void) )
{
	m_pMainLoopFunction =pFunction;
}

void Noise3D::Root::SetMainloopFunction(IMainloop * pInterface)
{
	m_pMainloopInterface = pInterface;
}

void Root::SetMainLoopStatus(NOISE_MAINLOOP_STATUS loopStatus)
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

bool Root::mFunction_InitWindowClass(WNDCLASS* wc)
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
		ERROR_MSG("Root: Create Window failed! Windows NT (or newer) required");
		return false;
	}
	else
	{
		return true;
	};

};

HWND Root::mFunction_InitWindow(UINT windowWidth,UINT windowHeight)
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

bool	Root::mFunction_CreateEffectFromMemory()
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


	HR_DEBUG(hr, "Root : load compiled shader failed");

	return true;
}


