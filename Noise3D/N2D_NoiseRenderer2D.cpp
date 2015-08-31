/************************************************************************

						CPP:  	Noise Renderer2D

					2D渲染，全部用fx来实现，d2d不好兼容

************************************************************************/
#pragma once
#include "Noise3D.h"

NoiseRenderer2D::NoiseRenderer2D()
{

};

NoiseRenderer2D::~NoiseRenderer2D()
{

};


BOOL	NoiseRenderer2D::mFunction_Init()
{
	HRESULT hr = S_OK;

	mFunction_Init_CreateEffectFromMemory("Main3D.fxo");

	//创建Technique
	m_pFX_Tech_Default = m_pFX->GetTechniqueByName("DefaultDraw");
	m_pFX_Tech_DrawLine3D = m_pFX->GetTechniqueByName("DrawLine3D");

	//然后要创建InputLayout
	//默认顶点
	D3DX11_PASS_DESC passDesc;
	m_pFX_Tech_Default->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice->CreateInputLayout(
		&g_VertexDesc_Default[0],
		g_VertexDesc_Default_ElementNum,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Default);
	HR_DEBUG(hr, "创建input Layout失败！");

	//simple顶点的
	m_pFX_Tech_DrawLine3D->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = g_pd3dDevice->CreateInputLayout(
		&g_VertexDesc_Simple[0],
		g_VertexDesc_Simple_ElementNum,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&g_pVertexLayout_Simple);
	HR_DEBUG(hr, "创建input Layout失败！");


	//创建Cbuffer
	m_pFX_CbPerFrame = m_pFX->GetConstantBufferByName("cbPerFrame");
	m_pFX_CbPerObject = m_pFX->GetConstantBufferByName("cbPerObject");
	m_pFX_CbPerSubset = m_pFX->GetConstantBufferByName("cbPerSubset");
	m_pFX_CbRarely = m_pFX->GetConstantBufferByName("cbRarely");
	m_pFX_CbDrawLine3D = m_pFX->GetConstantBufferByName("cbDrawLine3D");

	//纹理
	m_pFX_Texture_Diffuse = m_pFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_pFX_Texture_Normal = m_pFX->GetVariableByName("gNormalMap")->AsShaderResource();
	m_pFX_Texture_Specular = m_pFX->GetVariableByName("gSpecularMap")->AsShaderResource();

#pragma region CreateRasterState
	//创建预设的光栅化state
	//Create Raster State;If you want various Raster State,you should pre-Create all of them in the beginning
	D3D11_RASTERIZER_DESC tmpRasterStateDesc;//光栅化设置
	ZeroMemory(&tmpRasterStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	tmpRasterStateDesc.AntialiasedLineEnable = TRUE;//抗锯齿设置
	tmpRasterStateDesc.CullMode = D3D11_CULL_NONE;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullNone);
	HR_DEBUG(hr, "创建m_pRasterState_Solid_CullNone失败");

	tmpRasterStateDesc.CullMode = D3D11_CULL_NONE;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullNone);
	HR_DEBUG(hr, "创建m_pRasterState_WireFrame_CullNone失败");

	tmpRasterStateDesc.CullMode = D3D11_CULL_BACK;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullBack);
	HR_DEBUG(hr, "创建m_pRasterState_Solid_CullBack失败");

	tmpRasterStateDesc.CullMode = D3D11_CULL_BACK;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullBack);
	HR_DEBUG(hr, "创建m_pRasterState_WireFrame_CullBack失败");

	tmpRasterStateDesc.CullMode = D3D11_CULL_FRONT;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_SOLID;
	hr = g_pd3dDevice->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_Solid_CullFront);
	HR_DEBUG(hr, "创建m_pRasterState_Solid_CullFront失败");

	tmpRasterStateDesc.CullMode = D3D11_CULL_FRONT;//剔除模式
	tmpRasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = g_pd3dDevice->CreateRasterizerState(&tmpRasterStateDesc, &m_pRasterState_WireFrame_CullFront);
	HR_DEBUG(hr, "创建m_pRasterState_WireFrame_CullFront失败");

#pragma endregion CreateRasterState

	return TRUE;
};

BOOL	NoiseRenderer2D::mFunction_Init_CreateEffectFromFile(LPCWSTR fxPath)
{
	HRESULT hr = S_OK;

	DWORD shaderFlags = 0;
#if defined(DEBUG)||defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob*	compiledFX;
	ID3D10Blob*	compilationMsg;

	//编译fx文件
	hr = D3DX11CompileFromFile(
		fxPath, 0, 0, 0, "fx_5_0",
		shaderFlags, 0, 0, &compiledFX,
		&compilationMsg, 0);

	//看看编译有无错误信息
	//To see if there is compiling error
	if (compilationMsg != 0)
	{
		assert(FALSE:"Shader Compilation Failed !!");
		ReleaseCOM(compilationMsg);
	}


	hr = D3DX11CreateEffectFromMemory(
		compiledFX->GetBufferPointer(),
		compiledFX->GetBufferSize(),
		0, g_pd3dDevice, &m_pFX);
	HR_DEBUG(hr, "Create Basic Effect Fail!");
	ReleaseCOM(compiledFX);

	return TRUE;
};

BOOL	NoiseRenderer2D::mFunction_Init_CreateEffectFromMemory(char* compiledShaderPath)
{
	std::vector<char> compiledShader;

	//加载fxo文件
	if (!NoiseFileManager::ImportFile_PURE(compiledShaderPath, &compiledShader))
	{
		return FALSE;
	}

	//创建fx特效框架
	HRESULT hr = S_OK;
	hr = D3DX11CreateEffectFromMemory(&compiledShader[0], compiledShader.size(), 0, g_pd3dDevice, &m_pFX);
	HR_DEBUG(hr, "load compiled shader failed");

	//创建Technique
	m_pFX_Tech_Default = m_pFX->GetTechniqueByName("DefaultDraw");
	m_pFX_Tech_DrawLine3D = m_pFX->GetTechniqueByName("DrawLine3D");


	return TRUE;
};