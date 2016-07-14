
/***********************************************************************

                           h：NoiseGlobal

************************************************************************/


#pragma once
#include "Noise3D.h"

//#define NOISE_DLL_EXPORT /*_declspec(dllexport)*/

namespace Noise3D
{

	const UINT					g_VertexDesc_Default_ElementCount = 5;

	const UINT					g_VertexDesc_Simple_ElementCount = 3;

	//主渲染缓存的像素尺寸
	extern  UINT					gMainBufferPixelWidth;

	extern  UINT					gMainBufferPixelHeight;

	//抗锯齿品质
	extern UINT						g_Device_MSAA4xQuality;
	//抗锯齿是否开启
	extern BOOL						g_Device_MSAA4xEnabled;
	//
	extern D3D_DRIVER_TYPE			g_Device_driverType;
	//
	extern D3D_FEATURE_LEVEL		g_Device_featureLevel;

	//顶点描述
	extern D3D11_INPUT_ELEMENT_DESC g_VertexDesc_Default[g_VertexDesc_Default_ElementCount];

	extern D3D11_INPUT_ELEMENT_DESC g_VertexDesc_Simple[g_VertexDesc_Simple_ElementCount];

	//——————————全局接口——————————

	extern ID3D11Device*					g_pd3dDevice11;

	extern ID3D11DeviceContext*		g_pImmediateContext;
	//交换链
	extern IDXGISwapChain*				g_pSwapChain;
	//渲染视口 可以是数组 用于和pipeline绑定
	extern ID3D11RenderTargetView* g_pRenderTargetView;
	//深度&模版 只能有一个
	extern ID3D11DepthStencilView*	g_pDepthStencilView;
	//顶点布局
	extern ID3D11InputLayout*			g_pVertexLayout_Default;

	extern ID3D11InputLayout*			g_pVertexLayout_Simple;

	//——————————全局函数————————————
	extern /*_declspec(dllexport)*/ HRESULT gFunction_CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	extern /*_declspec(dllexport)*/ BOOL gFunction_IsPointInRect2D(NVECTOR2 v, NVECTOR2 vTopLeft, NVECTOR2 vBottomRight);

	extern /*_declspec(dllexport)*/ int	gFunction_GetCharAlignmentOffsetPixelY(UINT boundaryPxHeight, UINT charRealHeight, wchar_t inputChar);

	extern /*_declspec(dllexport)*/ UINT gFunction_MapDInputScanCodeToAscii(UINT scanCode, BOOL isCapital);

	extern /*_declspec(dllexport)*/ std::string GetFileDirectory(std::string completeFilePath);

	extern /*_declspec(dllexport)*/ float gFunction_Lerp(float a, float b, float t);

	extern /*_declspec(dllexport)*/ float gFunction_Clampf(float val, float min, float max);

	extern /*_declspec(dllexport)*/ int	gFunction_Clamp(int val, int min, int max);

	extern /*_declspec(dllexport)*/ void DEBUG_MSG1(std::string msg);

	extern /*_declspec(dllexport)*/ IRoot* GetRoot();

}