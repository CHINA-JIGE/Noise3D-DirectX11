
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
}
inline NVECTOR3 Noise3D::Ut::PixelCoordToDirection_SphericalMapping(int px, int py, int pixelWidth, int pixelHeight)
{
	float normalizedU = float(px) / float(pixelWidth);//[0,1]
	float normalizedV = float(py) / float(pixelHeight);//[0,1]

	//cube map's sampling is considered
	float yaw = (normalizedU - 0.5f) * 2.0f * Ut::PI;//[-pi,pi]
	float pitch = (normalizedV - 0.5f) * Ut::PI;//[pi/2,-pi/2]

	NVECTOR3 dir = Ut::YawPitchToDirection(yaw, pitch);
	//NVECTOR3 dir = { sinf(yaw)*cosf(pitch),  sinf(pitch) ,cosf(yaw)*cosf(pitch) };
	return dir;
}

inline void Noise3D::Ut::DirectionToPixelCoord_SphericalMapping(NVECTOR3 dir, int pixelWidth, int pixelHeight, uint32_t& outPixelX, uint32_t& outPixelY)
{
	//pitch, left-handed [-pi/2,pi/2]
	float pitch = 0.0f, yaw = 0.0f;
	Ut::DirectionToYawPitch(dir, yaw, pitch);

	//mapped to [0,1]
	float normalizedU = (yaw / (2.0f * Ut::PI)) + 0.5f;
	float normalizedV = (pitch / Ut::PI) + 0.5f;
	uint32_t x = uint32_t(float(pixelWidth) * normalizedU);
	uint32_t y = uint32_t(float(pixelHeight) * normalizedV);
	if (x == pixelWidth) x = pixelWidth - 1;
	if (y == pixelHeight) y = pixelHeight - 1;
	outPixelX = x;
	outPixelY = y;
}

inline void Noise3D::Ut::DirectionToYawPitch(NVECTOR3 dir, float& outYaw, float& outPitch)
{
	//yaw, start from z, left-handed
	outYaw = atan2(dir.x, dir.z);
	//pitch, left-handed [-pi/2,pi/2]
	outPitch = atan2(dir.y, sqrtf(dir.x*dir.x + dir.z*dir.z));
}

inline NVECTOR3 Noise3D::Ut::YawPitchToDirection(float yaw, float pitch)
{
	return  NVECTOR3(sinf(yaw)*cosf(pitch),  sinf(pitch) ,cosf(yaw)*cosf(pitch));
}

/*_declspec(dllexport)*/inline uint32_t Noise3D::Ut::Factorial32(uint32_t x)
{
	if (x == 0) return 1;
	uint32_t sum = 1;
	for (uint32_t i = 1; i <= x; ++i)
	{
		sum *= i;
	}
	return sum;
}

/*_declspec(dllexport)*/ uint64_t Noise3D::Ut::Factorial64(uint32_t x)
{
	if (x == 0) return 1;
	uint64_t sum = 1;
	for (uint64_t i = 1; i <= x; ++i)
	{
		sum *= i;
	}
	return sum;
}

/*_declspec(dllexport)*/ float Noise3D::Ut::ReciprocalOfFactorial(uint32_t x)
{
	const float table[] = {
		1.0f, //0
		1.0f,// 1
		0.5f,// 2
		0.16666666f,// 3
		0.04166666f, // 4
		0.008333333f, //5
		0.001388888f, //6
		0.00019841269f,//7
		0.00002480158f,//8
		0.00000275573f, //9
		2.75573192e-7f,//10
		2.50521084e-8f,//11
		2.0876757e-9f,//12
		1.6059044e-10f, //13
		1.1470746e-11f, //14
		7.6471637e-13f,//15
		4.7794773e-14f,//16
		2.8114573e-15f,//17
		1.5619207e-16f,//18
		8.2206352e-18f,//19
		4.1103176e-19f,//20
		1.9572941e-20f,//21
		8.8967914e-22f,//22
		3.8681702e-23f,//23
		1.6117376e-24f,//24
		6.4469503e-26f,//25
		2.4795963e-27f,//26
		9.1836899e-29f,//27
		3.2798892e-30f,//28
		1.1309963e-31f,//29
		3.7699876e-33f,//30
		1.216125e-34f,//31
		3.8003908e-36f,//32
	};

	if (x <= 32)return table[x];
	else
	{
		//divide multiple times to prevent drastic floating-point error
		float result = 1.0f;
		for (uint32_t i = 1; i <= x; ++i)
		{
			result /= float(i);
		}
		return result;
	}
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

/*_declspec(dllexport)*/ NVECTOR4 Noise3D::Ut::Clamp(const NVECTOR4 & target, const NVECTOR4 & min, const NVECTOR4 & max)
{
	return NVECTOR4(Clamp(target.x, min.x, max.x), Clamp(target.y, min.y, max.y), Clamp(target.z, min.z, max.z), Clamp(target.w,min.w,max.w));
}

/*_declspec(dllexport)*/ NColor4f Noise3D::Ut::Clamp(const NColor4f & target, const NColor4f & min, const NColor4f & max)
{
	return NColor4f(Clamp(target.x, min.x, max.x), Clamp(target.y, min.y, max.y), Clamp(target.z, min.z, max.z), Clamp(target.w, min.w, max.w));
}

/*_declspec(dllexport)*/  inline NVECTOR3 Noise3D::Ut::CubicHermite(const NVECTOR3 & v1, const NVECTOR3 & v2, const NVECTOR3 & t1, const NVECTOR3 & t2, float t)
{
	//https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline
	return (2.0f * t * t * t - 3.0f * t * t + 1.0f) * v1 + (t * t * t - 2.0f * t * t + t) * t1 + (-2.0f * t * t * t + 3.0f * t * t) * v2 + (t * t * t - t * t) * t2;
}

/*_declspec(dllexport)*/ bool Noise3D::Ut::TolerantEqual(float lhs, float rhs, float errorLimit)
{
	return (abs(lhs - rhs) < errorLimit);
}

/*_declspec(dllexport)*/uint32_t Noise3D::Ut::ComputeMipMapChainPixelCount(uint32_t mipLevel, uint32_t width, uint32_t height)
{
	//calculate the pitch of one mipmap chain
	// sum(1,2,4,8....,2^n)=a_1(1-q^n)/(1-q) = 2^n-1
	uint32_t sum = 0;
	uint32_t tmpWidth = width;
	uint32_t tmpHeight = height;
	for (uint32_t i = 0; i < mipLevel; ++i)
	{
		sum += tmpWidth * tmpHeight;
		tmpWidth /= 2;
		tmpHeight /= 2;
	}
	return  sum;
}

bool Noise3D::Ut::Debug_ComPtrBatchDestructionWithHResultDebug(HRESULT hr, const std::string& MsgText,int ptrCount, IUnknown* ptrArr...)
{
	//similar function to MACRO HR_DEBUG, but this function support destruction of other pointers
	//(when init failed, previously created pointer should be deleted)
	//use variable templates to support multiple parameters...(with standard c++)

	va_list   arg_ptr;
	va_start(arg_ptr, ptrCount);   //init with (va_list, last fixed parameter)

	if (FAILED(hr)) 
	{
		ERROR_MSG("d3d returned error code : " + std::to_string(hr) + "\n" + MsgText); 
		for (int i = 0; i < ptrCount; ++i)
		{
			IUnknown* pComPtr = va_arg(arg_ptr, IUnknown*);
			ReleaseCOM(pComPtr);
		}
		return false; 
	}
	else
	{
		return true;
	}
}

void Noise3D::Ut::Debug_ComPtrBatchDestruction(int ptrCount, IUnknown *ptrArr ...)
{
	va_list   arg_ptr;
	va_start(arg_ptr, ptrCount);   //init with (va_list, last fixed parameter)

	for (int i = 0; i < ptrCount; ++i)
	{
		IUnknown* pComPtr = va_arg(arg_ptr, IUnknown*);
		ReleaseCOM(pComPtr);
	}
};


