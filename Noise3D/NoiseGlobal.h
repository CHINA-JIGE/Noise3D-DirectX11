
/***********************************************************************

                           h：NoiseGlobal

************************************************************************/


#pragma once
#include "Noise3D.h"

CONST UINT32					g_VertexDesc_ElementNum = 4;

//抗锯齿品质
extern UINT						g_Device_MSAA4xQuality ;
//抗锯齿是否开启
extern BOOL						g_Device_MSAA4xEnabled ;
//
extern D3D_DRIVER_TYPE			g_Device_driverType;
//
extern D3D_FEATURE_LEVEL		g_Device_featureLevel;
//弹出数字的MSG
extern std::ostringstream		g_Debug_MsgString;
//顶点描述
extern D3D11_INPUT_ELEMENT_DESC g_VertexDesc_Default[g_VertexDesc_ElementNum];

//——————————全局接口——————————

extern ID3D11Device*           g_pd3dDevice;

extern ID3D11DeviceContext*    g_pImmediateContext;
//交换链
extern IDXGISwapChain*         g_pSwapChain;
//渲染视口 可以是数组 用于和pipeline绑定
extern ID3D11RenderTargetView* g_pRenderTargetView;
//深度&模版 只能有一个
extern ID3D11DepthStencilView*	g_pDepthStencilView;
//顶点布局
extern ID3D11InputLayout*      g_pVertexLayout;

//extern ID3D11VertexShader*		g_pDefaultVertexShader;

//extern ID3D11PixelShader*			g_pDefaultPixelShader;
//光栅化设置
extern ID3D11RasterizerState*		g_pRasterState_FillMode_Solid ;
extern ID3D11RasterizerState*		g_pRasterState_FillMode_WireFrame;

//——————————全局函数————————————

extern HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );