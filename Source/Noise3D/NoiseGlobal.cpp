
/***********************************************************************

							Global Variables

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

	/*--------------------------全局变量----------------------------*/

	//主缓存的像素宽度
	UINT		Noise3D::gMainBufferPixelWidth = 640;
	//主缓存的像素高度
	UINT		Noise3D::gMainBufferPixelHeight = 480;

	//抗锯齿品质
	UINT				Noise3D::g_Device_MSAA4xQuality = 1;
	//抗锯齿是否开启
	BOOL				Noise3D::g_Device_MSAA4xEnabled = FALSE;
	//只是初始化设定是这样而已
	D3D_DRIVER_TYPE		Noise3D::g_Device_driverType = D3D_DRIVER_TYPE_SOFTWARE;
	//
	D3D_FEATURE_LEVEL	Noise3D::g_Device_featureLevel = D3D_FEATURE_LEVEL_11_0;

	//SemanticName,Index,Format,InputSlot,ByteOffset,InputSlotClass,InstanceDataStepRate
	D3D11_INPUT_ELEMENT_DESC Noise3D::g_VertexDesc_Simple[] =
	{
		{"POSITION",0,	DXGI_FORMAT_R32G32B32_FLOAT,		0,0,	D3D11_INPUT_PER_VERTEX_DATA,0},
		{  "COLOR",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,12,	D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "TEXCOORD",0,	DXGI_FORMAT_R32G32_FLOAT,		0,28,	D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	D3D11_INPUT_ELEMENT_DESC Noise3D::g_VertexDesc_Default[] =
	{
		{"POSITION",0,	DXGI_FORMAT_R32G32B32_FLOAT,		0,0,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"COLOR",0,		DXGI_FORMAT_R32G32B32A32_FLOAT,	0,12,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"NORMAL",0,	DXGI_FORMAT_R32G32B32_FLOAT,		0,28,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"TEXCOORD",0,	DXGI_FORMAT_R32G32_FLOAT,		0,40,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{ "TANGENT",0,	DXGI_FORMAT_R32G32_FLOAT,			0,48,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
	};



	/*-----------------------------全局接口-----------------------------*/

	ID3D11Device*           Noise3D::g_pd3dDevice11 = NULL;

	ID3D11DeviceContext*		Noise3D::g_pImmediateContext = NULL;
	//交换链
	IDXGISwapChain*				Noise3D::g_pSwapChain = NULL;
	//渲染视口 可以是数组 用于和pipeline绑定
	ID3D11RenderTargetView* Noise3D::g_pRenderTargetView = NULL;
	//深度&模版 只能有一个
	ID3D11DepthStencilView*	 Noise3D::g_pDepthStencilView = NULL;
	//顶点布局
	ID3D11InputLayout*			Noise3D::g_pVertexLayout_Default = NULL;
	ID3D11InputLayout*			Noise3D::g_pVertexLayout_Simple = NULL;

	/*------------------------------全局函数--------------------------*/

/*_declspec(dllexport)*/  HRESULT Noise3D::gFunction_CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
	{

		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* pErrorBlob;
		hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
		if (FAILED(hr))
		{
			if (pErrorBlob != NULL)
				OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			if (pErrorBlob) pErrorBlob->Release();
			return hr;
		}
		if (pErrorBlob) pErrorBlob->Release();

		return S_OK;
	};

/*_declspec(dllexport)*/	BOOL Noise3D::gFunction_IsPointInRect2D(NVECTOR2 v, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight)
	{
		if (v.x >= vTopLeft.x &&
			v.x <= vBottomRight.x &&
			v.y >= vTopLeft.y &&
			v.y <= vBottomRight.y)
		{
			return TRUE;
		}

		return FALSE;
	}

/*_declspec(dllexport)*/ int Noise3D::gFunction_GetCharAlignmentOffsetPixelY(UINT boundaryPxHeight, UINT charRealHeight, wchar_t inputChar)
	{
		//!!!!!!the return type is signed INT ,because  top left might go beyond the upper boundary

	   //Ascii alignment
		switch (inputChar)
		{
			//1, upper alignment ,the top of each character lies right beneath the top
		case '\"':	 case '\'':	 case '^':	 case '*':	 case '`':	case'j':
			return 0;
			break;

			//2,lower-bound alignment
		case L'，':  case L'。': case L'；': case L'：':  case L'？':  case L'、':  case L'《':
		case L'》':  case L'”': case L'“':  case L'’':	case L'·':	case L'【':  case L'】':
		case L'｛':  case L'｝': case L'‘':  case L'—':
		case '[':	case ']':		case '{':	case'}': case '|':
			return (int(boundaryPxHeight *0.75) - charRealHeight);
			break;

			//3,upper 1/3 alignment
		case 'g':	case 'p':	case 'q': case 'y': case '-': case '=': //case '+':
			return boundaryPxHeight / 4;
			break;

			//4, lower 2/3 alignment, the bottom of each character lies right on the 2/3 line
		default:
			if (inputChar > 0x007f)
			{
				return int((boundaryPxHeight *0.5) - charRealHeight*0.5);//CHINESE,middle alignment
			}
			else
			{
				return (int(boundaryPxHeight *0.75) - charRealHeight);
			}
			break;

		}

		return 0;
	}

/*_declspec(dllexport)*/ UINT Noise3D::gFunction_MapDInputScanCodeToAscii(UINT scanCode, BOOL isCapital)
	{
#define returnChar(UCase,LCase) if(isCapital){return UCase;}else {return LCase;} break;

		switch (scanCode)
		{
		case NOISE_KEY_0:	returnChar(')', '0');
		case NOISE_KEY_1:	returnChar('!', '1');
		case NOISE_KEY_2:	returnChar('@', '2');
		case NOISE_KEY_3:	returnChar('#', '3');
		case NOISE_KEY_4:	returnChar('$', '4');
		case NOISE_KEY_5:	returnChar('%', '5');
		case NOISE_KEY_6:	 returnChar('^', '6');
		case NOISE_KEY_7:	returnChar('&', '7');
		case NOISE_KEY_8:	returnChar('*', '8');
		case NOISE_KEY_9:	returnChar('(', '9');
		case NOISE_KEY_A:	returnChar('A', 'a');
		case NOISE_KEY_B:	returnChar('B', 'b');
		case NOISE_KEY_C:	returnChar('C', 'c');
		case NOISE_KEY_D:	returnChar('D', 'd');
		case NOISE_KEY_E:	returnChar('E', 'e');
		case NOISE_KEY_F:	returnChar('F', 'f');
		case NOISE_KEY_G:	returnChar('G', 'g');
		case NOISE_KEY_H:	returnChar('H', 'h');
		case NOISE_KEY_I:	returnChar('I', 'i');
		case NOISE_KEY_J:	returnChar('J', 'j');
		case NOISE_KEY_K:	returnChar('K', 'k');
		case NOISE_KEY_L:	returnChar('L', 'l');
		case NOISE_KEY_M:	returnChar('M', 'm');
		case NOISE_KEY_N:	returnChar('N', 'n');
		case NOISE_KEY_O:	returnChar('O', 'o');
		case NOISE_KEY_P:	returnChar('P', 'p');
		case NOISE_KEY_Q:	returnChar('Q', 'q');
		case NOISE_KEY_R:	returnChar('R', 'r');
		case NOISE_KEY_S:	returnChar('S', 's');
		case NOISE_KEY_T:	returnChar('T', 't');
		case NOISE_KEY_U:	returnChar('U', 'u');
		case NOISE_KEY_V:	returnChar('V', 'v');
		case NOISE_KEY_W:	returnChar('W', 'w');
		case NOISE_KEY_X:	returnChar('X', 'x');
		case NOISE_KEY_Y:	returnChar('Y', 'y');
		case NOISE_KEY_Z:	returnChar('Z', 'z');
		case NOISE_KEY_COMMA:	returnChar('<', ',');
		case NOISE_KEY_PERIOD:		returnChar('>', '.');
		case NOISE_KEY_LBRACKET:	returnChar('{', '[');
		case NOISE_KEY_RBRACKET:	returnChar('}', ']');
		case NOISE_KEY_MINUS:		returnChar('_', '-');
		case NOISE_KEY_EQUALS:	returnChar('+', '=');
		case NOISE_KEY_SEMICOLON: returnChar(':', ';');
		case NOISE_KEY_SLASH: returnChar('?', '/');
		case NOISE_KEY_BACKSLASH: returnChar('|', '\\');
		case NOISE_KEY_APOSTROPHE: returnChar('"', '\'');
		case NOISE_KEY_SPACE: return ' ';break;
		default:return 0;break;
		}
	};

/*_declspec(dllexport)*/ std::string Noise3D::GetFileDirectory(std::string completeFilePath)
	{
		//Get the directory which the file lies on 
		std::string outDir = completeFilePath;
		while (outDir.size() > 0)
		{
			if (outDir.back() != '\\' && outDir.back() != '/')
			{
				outDir.pop_back();
			}
			else
			{
				return outDir;
			}
		}
		return "";
	};

/*_declspec(dllexport)*/ inline float Noise3D::gFunction_Lerp(float a, float b, float t)
	{
		return(a + (b - a)*t);
	};

/*_declspec(dllexport)*/ inline float Noise3D::gFunction_Clampf(float val, float min, float max)
	{
		return (val >= min ? (val <= max ? val : max) : min);
	};

/*_declspec(dllexport)*/ inline int	Noise3D::gFunction_Clamp(int val, int min, int max)
	{
		return (val >= min ? (val <= max ? val : max) : min);
	};

/*_declspec(dllexport)*/ inline void Noise3D::DEBUG_MSG1(std::string msg)
	{
		std::ostringstream debugMsg;
		debugMsg << msg << std::endl;
		debugMsg << "file: " << __FILE__ << std::endl;
		debugMsg << "line: " << __LINE__ << std::endl;
		debugMsg << "function:" << __func__ << std::endl;
		MessageBoxA(0, debugMsg.str().c_str(), 0, 0);
		debugMsg.clear();
	};

/*_declspec(dllexport)*/ IRoot* Noise3D::GetRoot()
{
	class IRootCreation :public IFactory<IRoot>
	{
	public:

		IRootCreation() :IFactory<IRoot>(1) {};

		~IRootCreation() { delete m_pRoot; }

		IRoot* CreateRoot()
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
	static IRoot* ptr = rootCreationFactory.CreateRoot();
	return  ptr;
};
