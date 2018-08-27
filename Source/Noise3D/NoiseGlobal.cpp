
/***********************************************************************

							Global Variables

***********************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

	/*--------------------------全局变量----------------------------*/

	//只是初始化设定是这样而已
	D3D_DRIVER_TYPE		Noise3D::D3D::g_Device_driverType = D3D_DRIVER_TYPE_HARDWARE;
	//
	D3D_FEATURE_LEVEL	Noise3D::D3D::g_Device_featureLevel = D3D_FEATURE_LEVEL_11_0;

	//SemanticName,Index,Format,InputSlot,ByteOffset,InputSlotClass,InstanceDataStepRate
	D3D11_INPUT_ELEMENT_DESC Noise3D::D3D::g_VertexDesc_Simple[] =
	{
		{"POSITION",0,	DXGI_FORMAT_R32G32B32_FLOAT,		0,0,	D3D11_INPUT_PER_VERTEX_DATA,0},
		{  "COLOR",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT,	0,12,	D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "TEXCOORD",0,	DXGI_FORMAT_R32G32_FLOAT,		0,28,	D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	D3D11_INPUT_ELEMENT_DESC Noise3D::D3D::g_VertexDesc_Default[] =
	{
		{"POSITION",0,		DXGI_FORMAT_R32G32B32_FLOAT,		0,0,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"COLOR",0,			DXGI_FORMAT_R32G32B32A32_FLOAT,	0,12,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"NORMAL",0,		DXGI_FORMAT_R32G32B32_FLOAT,		0,28,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"TEXCOORD",0,	DXGI_FORMAT_R32G32_FLOAT,			0,40,	D3D11_INPUT_PER_VERTEX_DATA,	0},
		{ "TANGENT",0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,48,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
	};


	/*-----------------------------全局接口-----------------------------*/

	ID3D11Device*					Noise3D::D3D::g_pd3dDevice11 = nullptr;

	ID3D11DeviceContext*		Noise3D::D3D::g_pImmediateContext = nullptr;

	ID3DX11Effect*					Noise3D::D3D::g_pFX = nullptr;

	//顶点布局
	ID3D11InputLayout*			Noise3D::D3D::g_pVertexLayout_Default = nullptr;
	ID3D11InputLayout*			Noise3D::D3D::g_pVertexLayout_Simple = nullptr;

	/*------------------------------Global Function--------------------------*/

/*_declspec(dllexport)*/	bool Noise3D::Ut::IsPointInRect2D(NVECTOR2 v, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight)
	{
		if (v.x >= vTopLeft.x &&
			v.x <= vBottomRight.x &&
			v.y >= vTopLeft.y &&
			v.y <= vBottomRight.y)
		{
			return true;
		}

		return false;
	}

/*_declspec(dllexport)*/ int Noise3D::Ut::GetCharAlignmentOffsetPixelY(UINT boundaryPxHeight, UINT charRealHeight, wchar_t inputChar)
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


/*_declspec(dllexport)*/ std::string Noise3D::Ut::GetFileFolderFromPath(std::string completeFilePath)
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

/*_declspec(dllexport)*/ std::string Noise3D::Ut::GetFileNameFromPath(std::string completeFilePath)
{
	//Get the directory which the file lies on 
	//Note that if find_last_of() failed, a int(-1) will be returned
	int pos1 = completeFilePath.find_last_of('/');
	int pos2 = completeFilePath.find_last_of('\\');
	std::string result = completeFilePath.substr(max(pos1, pos2) + 1);
	return result;
}

/*_declspec(dllexport)*/ std::string Noise3D::Ut::GetFileSubFixFromPath(std::string path)
{
	//Get the directory which the file lies on 
	size_t pos = path.find_last_of('.');
	if (pos == std::string::npos)return "";//'.' not found, no subfix
	std::string result = path.substr(pos+1);
	return result;
}

//in some cases, wstring is needed. 
/*_declspec(dllexport)*/  std::wstring Noise3D::Ut::ConvertAnsiStrToWStr(std::string srcStr)
{
	std::wstring wstr(srcStr.size(),L'\0');
	for (auto e : srcStr)wstr.push_back(e);
	return wstr;
};

/*_declspec(dllexport)*/ inline float Noise3D::Ut::Lerp(float a, float b, float t)
	{
		return(a + (b - a)*t);
	}

/*_declspec(dllexport)*/ inline NVECTOR2 Noise3D::Ut::Lerp(NVECTOR2 a, NVECTOR2 b, float t)
{
	return NVECTOR2(Lerp(a.x,b.x,t),Lerp(a.y,b.y,t));
}

/*_declspec(dllexport)*/ inline NVECTOR3 Noise3D::Ut::Lerp(NVECTOR3 v1, NVECTOR3 v2, float t)
{
	return NVECTOR3(Lerp(v1.x,v2.x,t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t));
};

/*_declspec(dllexport)*/ inline float Noise3D::Ut::Clamp(float val, float min, float max)
	{
		return (val >= min ? (val <= max ? val : max) : min);
	};

/*_declspec(dllexport)*/ inline int	Noise3D::Ut::Clamp(int val, int min, int max)
{
	return (val >= min ? (val <= max ? val : max) : min);
}

/*_declspec(dllexport)*/ inline double Noise3D::Ut::Clamp(double val, double min, double max)
{
	return (val >= min ? (val <= max ? val : max) : min);
}

/*_declspec(dllexport)*/ inline NVECTOR3 Noise3D::Ut::Clamp(const NVECTOR3 & target, const NVECTOR3 & min, const NVECTOR3 & max)
{
	return NVECTOR3(Clamp(target.x,min.x,max.x), Clamp(target.y, min.y, max.y),Clamp(target.z, min.z, max.z));
}

/*_declspec(dllexport)*/  inline NVECTOR3 Noise3D::Ut::CubicHermite(const NVECTOR3 & v1, const NVECTOR3 & v2, const NVECTOR3 & t1, const NVECTOR3 & t2, float t)
{
	//https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline
	return (2.0f * t * t * t - 3.0f * t * t + 1.0f) * v1 + (t * t * t - 2.0f * t * t + t) * t1 + (-2.0f * t * t * t + 3.0f * t * t) * v2 + (t * t * t - t * t) * t2;
};


