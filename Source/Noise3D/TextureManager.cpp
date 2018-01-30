
/***********************************************************************

							class£ºNOISE texture Manger

							texture management/modification

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

ITextureManager::ITextureManager():
	IFactory<ITexture>(50000)//maxCount of Textures
{

};

ITextureManager::~ITextureManager()
{
	DeleteAllTexture();
}

//--------------------------------TEXTURE CREATION-----------------------------
ITexture* ITextureManager::CreatePureColorTexture(N_UID texName, UINT pixelWidth, UINT pixelHeight, NVECTOR4 color, bool keepCopyInMemory)
{
	//create New Texture Object
	HRESULT hr = S_OK;

	//we must check if new name has been used
	if(ValidateUID(texName)== true)
	{
			ERROR_MSG("CreateTextureFromFile : Texture name has been used!! name: " + texName);
			return nullptr;//invalid
	}

#pragma region CreateTex2D&SRV

	//a temporary Texture Object
	/*N_TextureObject tmpTexObj;
	tmpTexObj.mIsPixelBufferInMemValid = keepCopyInMemory;
	tmpTexObj.mTexName = tmpStringTextureName;
	tmpTexObj.mTextureType = NOISE_TEXTURE_TYPE_COMMON;*/

	//assign a lot of same NVECTOR4 color to vector
	std::vector<NVECTOR4> initPixelBuffer(pixelWidth*pixelHeight, color);


	//texture2D desc (create a default usage texture)
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = pixelWidth;//determined by the loaded picture
	texDesc.Height = pixelHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;//allow update subresource
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = NULL;
	texDesc.MiscFlags = NULL;

	//SRV desc
	D3D11_SHADER_RESOURCE_VIEW_DESC SRViewDesc;
	SRViewDesc.Format = texDesc.Format;
	SRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
	SRViewDesc.TextureCube.MostDetailedMip = 0;

	//init data
	D3D11_SUBRESOURCE_DATA texInitDataDesc;
	texInitDataDesc.pSysMem = &initPixelBuffer.at(0);
	texInitDataDesc.SysMemPitch = (texDesc.Width) * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE;//bytesize for 1 row
	texInitDataDesc.SysMemSlicePitch = 0;//available for tex3D

	 //create texture2D first 
	ID3D11Texture2D* pTmpTexture2D;
	hr = g_pd3dDevice11->CreateTexture2D(&texDesc, &texInitDataDesc, &pTmpTexture2D);
	if (FAILED(hr))
	{
		ReleaseCOM(pTmpTexture2D);
		ERROR_MSG("CreateTextureFromFile : Create ID3D11Texture2D failed!");
		return nullptr;
	}

	//Create SRV from texture 2D (to a tmp textureObject)
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;
	hr = g_pd3dDevice11->CreateShaderResourceView(pTmpTexture2D, &SRViewDesc, &tmp_pSRV);
	if (FAILED(hr))
	{
		ReleaseCOM(pTmpTexture2D);
		ERROR_MSG("CreateTextureFromFile : Create ID3D11SRV failed!");
		return nullptr;
	}


	//clear tmp interfaces
	ReleaseCOM(pTmpTexture2D);

	//clear Memory of the picture
	if (!keepCopyInMemory)initPixelBuffer.clear();

#pragma endregion CreateTex2D&SRV

	//at last push back a new texture object
	ITexture* pTexObj= IFactory<ITexture>::CreateObject(texName);
	pTexObj->mFunction_InitTexture(tmp_pSRV, texName, std::move(initPixelBuffer), keepCopyInMemory, NOISE_TEXTURE_TYPE_COMMON);

	return pTexObj;
};

ITexture* ITextureManager::CreateTextureFromFile(NFilePath filePath, N_UID texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight,bool keepCopyInMemory)
{

	if (keepCopyInMemory)
	{
		return mFunction_CreateTextureFromFile_KeepACopyInMemory(filePath, texName, useDefaultSize, pixelWidth, pixelHeight);
	}
	else
	{
		return mFunction_CreateTextureFromFile_DirectlyLoadToGpu(filePath, texName, useDefaultSize, pixelWidth, pixelHeight);
	}
	return nullptr;
}

ITexture* ITextureManager::CreateCubeMapFromFiles(NFilePath fileName[6], N_UID cubeTextureName, NOISE_CUBEMAP_SIZE faceSize)
{
	HRESULT hr = S_OK;


#pragma region LoadDataToBufferArray
	//create temporary textures , so we should mark the ID to delete it later
	std::string tmpTexName[6];
	UINT cubeMapWidth = 0;

	//...what size to use
	switch (faceSize)
	{
	case NOISE_CUBEMAP_SIZE_64x64:
		cubeMapWidth = 64;
		break;
	case NOISE_CUBEMAP_SIZE_128x128:
		cubeMapWidth = 128;
		break;
	case NOISE_CUBEMAP_SIZE_256x256:
		cubeMapWidth = 256;
		break;
	case NOISE_CUBEMAP_SIZE_512x512:
		cubeMapWidth = 512;
		break;
	case NOISE_CUBEMAP_SIZE_1024x1024:
		cubeMapWidth = 1024;
		break;
	};

	//create temporary textures
	for (UINT i = 0; i < 6;i++)
	{
		 tmpTexName[i] = "cubeMapTmpTexure" +std::to_string(i);
		ITexture* pTexture = CreateTextureFromFile(
			fileName[i], 
			tmpTexName[i].c_str(),
			false,
			cubeMapWidth, 
			cubeMapWidth, 
			true);

		//one of the texture failed loading
		if (pTexture==nullptr)
		{
			for (UINT j = 0;j <= i;j++)
			{
				//delete previously created temporary textures
				DeleteTexture(tmpTexName[j]);
			}

			ERROR_MSG("ITextureMgr :CreateCubeMapFromFiles:create face from file failed ! face ID : ");
			return nullptr;
		}
	}


	//combine 4 buffers
	std::vector<NVECTOR4> pixelBuffer[6];
	for (UINT i = 0;i < 6;i++)
	{
		auto srcPartialBuffer = IFactory<ITexture>::GetObjectPtr(tmpTexName[i])->mPixelBuffer;
		//assign values for each buffer
		pixelBuffer[i].assign(srcPartialBuffer.begin(), srcPartialBuffer.end());
	}

	//delete temporary textures after copying data to pixelBuffer(s)
	for (UINT k = 0;k < 6;k++)DeleteTexture(tmpTexName[k]);

#pragma endregion LoadDataToBufferArray


#pragma region CreateCubeMap
	//we must check if new texture name has been used
	/*std::string cubemapNameString(cubeTextureName);
	for (auto t : *m_pTextureObjectList)
	{
		if (t.mTexName == cubemapNameString)
		{
			DEBUG_MSG1("NoiseTexMgr:CreateCubeMapFromFiles : Texture name has been used!!");
			return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
		}
	}*/


	//cube map is similar to texture arrays
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRViewDesc;
	D3D11_SUBRESOURCE_DATA texInitDataDesc[6];
	for (UINT i = 0;i < 6;i++)
	{
		texInitDataDesc[i].pSysMem = &pixelBuffer[i].at(0);
		texInitDataDesc[i].SysMemPitch = cubeMapWidth * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE;
		texInitDataDesc[i].SysMemSlicePitch = 0;
	}

	//2 description need to be updated
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = cubeMapWidth;
	texDesc.Height = cubeMapWidth;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ZeroMemory(&SRViewDesc, sizeof(SRViewDesc));
	SRViewDesc.Format = texDesc.Format;
	SRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SRViewDesc.TextureCube.MipLevels = texDesc.MipLevels;


	//create id3d11texture2D
	ID3D11Texture2D* pCubeMapTexture2D;
	hr = g_pd3dDevice11->CreateTexture2D(&texDesc, &texInitDataDesc[0], &pCubeMapTexture2D);
	if (FAILED(hr))
	{
		ERROR_MSG("NoiseTexMgr:CreateCubeMapFromFiles : Create new cube map failed!!");
		return nullptr;
	}

	
	//we disable texture modification of Cube Map for the time being...
	/*N_TextureObject tmpTexObj;
	tmpTexObj.mIsPixelBufferInMemValid = false;
	tmpTexObj.mTextureType = NOISE_TEXTURE_TYPE_CUBEMAP;*/

	//create SRV from texture2D
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;
	hr = g_pd3dDevice11->CreateShaderResourceView(
		pCubeMapTexture2D,
		&SRViewDesc,
		&tmp_pSRV);

	if (FAILED(hr))
	{
		//new temporary tex obj should be dumped
		ERROR_MSG("NoiseTexMgr:CreateCubeMapFromFiles : Create SRV failed!!");
		return nullptr;
	}

#pragma endregion CreateCubeMap

	//Create a new Texture object
	ITexture* pTexObj = IFactory<ITexture>::CreateObject(cubeTextureName);
	std::vector<NVECTOR4> emptyBuff;
	pTexObj->mFunction_InitTexture(tmp_pSRV, cubeTextureName, std::move(emptyBuff), false, NOISE_TEXTURE_TYPE_CUBEMAP);

	return pTexObj;
}

ITexture* ITextureManager::CreateCubeMapFromDDS(NFilePath dds_FileName, N_UID cubeTextureName, NOISE_CUBEMAP_SIZE faceSize)
{
	bool isFileNameValid;
	for (UINT i = 0; i < 6;i++)
	{
		//try opening a file
		isFileNameValid = std::fstream(dds_FileName).good();

		//check if  one of the texture id is illegal
		if (!isFileNameValid)
		{
			ERROR_MSG("Noise Tex Mgr :CreateCubeTextureFromDDS : file not exist!! ; Index : ");
			return nullptr;
		}
	}

#pragma region CreateSRVFromDDS

	//some settings about loading image
	D3DX11_IMAGE_LOAD_INFO loadInfo;

	//we must check if user want to use default image size
	switch(faceSize)
	{
	case NOISE_CUBEMAP_SIZE_64x64:
		loadInfo.Width = 64;
		loadInfo.Height = 64;
		break;
	case NOISE_CUBEMAP_SIZE_128x128:
		loadInfo.Width = 128;
		loadInfo.Height = 128;
		break;
	case NOISE_CUBEMAP_SIZE_256x256:
		loadInfo.Width = 256;
		loadInfo.Height = 256;
		break;
	case NOISE_CUBEMAP_SIZE_512x512:
		loadInfo.Width = 512;
		loadInfo.Height = 512;
		break;
	case NOISE_CUBEMAP_SIZE_1024x1024:
		loadInfo.Width = 1024;
		loadInfo.Height = 1024;
		break;
	};
	

	//continue filling the settings
	loadInfo.Filter = D3DX11_FILTER_LINEAR;
	loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	loadInfo.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	//create New Texture Object
	HRESULT hr = S_OK;

	//then endow a pointer to new SRV
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;
	D3DX11CreateShaderResourceViewFromFileA(
		g_pd3dDevice11,
		dds_FileName.c_str(),
		&loadInfo,
		nullptr,
		&tmp_pSRV,
		&hr
		);

	//................
	if (FAILED(hr))
	{
		ERROR_MSG("CreateCubeMapFromDDS : Create SRV failed!");
		return nullptr;
	}


#pragma endregion CreateSRVFromDDS

	//Create a new Texture object
	ITexture* pTexObj = IFactory<ITexture>::CreateObject(cubeTextureName);
	std::vector<NVECTOR4> emptyBuff;
	pTexObj->mFunction_InitTexture(tmp_pSRV, cubeTextureName, std::move(emptyBuff), false, NOISE_TEXTURE_TYPE_CUBEMAP);

	//return new texObj ptr
	return pTexObj;
}

ITexture * ITextureManager::GetTexture(N_UID texName)
{
	return IFactory<ITexture>::GetObjectPtr(texName);
}

ITexture * Noise3D::ITextureManager::BakeLightMapForMesh(IMesh * pMesh)
{
	ERROR_MSG("Light map baking hasn't implemented!");
	return nullptr;
}

UINT	 ITextureManager::GetTextureCount()
{
	return IFactory<ITexture>::GetObjectCount();
}

bool ITextureManager::DeleteTexture(ITexture * pTex)
{
	if (pTex == nullptr)
	{
		return false;
	}
	else
	{
		return IFactory<ITexture>::DestroyObject(pTex->GetTextureName());
	}
}

bool ITextureManager::DeleteTexture(N_UID uid)
{
	auto texturePtr = IFactory<ITexture>::GetObjectPtr(uid);
	return IFactory<ITexture>::DestroyObject(uid);
}

void ITextureManager::DeleteAllTexture()
{
	IFactory<ITexture>::DestroyAllObject();
}

bool ITextureManager::ValidateUID(N_UID texName)
{
	return IFactory<ITexture>::FindUid(texName);
}

bool ITextureManager::ValidateUID(N_UID texName, NOISE_TEXTURE_TYPE texType)
{
	if (IFactory<ITexture>::FindUid(texName) == false)
	{
		return false;
	}

	//then check the texture type : common / cubemap / volumn
	D3D11_SHADER_RESOURCE_VIEW_DESC tmpSRViewDesc;
	IFactory<ITexture>::GetObjectPtr(texName)->m_pSRV->GetDesc(&tmpSRViewDesc);

	switch (tmpSRViewDesc.ViewDimension)
	{
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		if (texType != NOISE_TEXTURE_TYPE_CUBEMAP)
		{
			return false;
		}
		break;

	case D3D11_SRV_DIMENSION_TEXTURE3D:
		if (texType != NOISE_TEXTURE_TYPE_VOLUMN)
		{
			return false;
		}
		break;

	case D3D11_SRV_DIMENSION_TEXTURE2D:
		if (texType != NOISE_TEXTURE_TYPE_COMMON)
		{
			return false;
		}
		break;

	}

	//a no-problem texID
	return true;
}


/*************************************************************
							P R I V A T E
*************************************************************/

ITexture* ITextureManager::mFunction_CreateTextureFromFile_DirectlyLoadToGpu(NFilePath filePath, std::string& texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight)
{
	//!!!!!!!!!!!!!!!!File Size Maybe a problem???

	//check if the file existed
	std::fstream tmpFile(filePath, std::ios::binary | std::ios::in);
	if (tmpFile.bad())
	{
		ERROR_MSG("Texture file not found!!");
		return nullptr;//invalid
	}

	//some settings about loading image
	D3DX11_IMAGE_LOAD_INFO loadInfo;

	//we must check if user want to use default image size
	if (useDefaultSize)
	{
		loadInfo.Width = D3DX11_DEFAULT;
		loadInfo.Height = D3DX11_DEFAULT;
	}
	else
	{
		loadInfo.Width = (pixelWidth > 0 ? pixelWidth : D3DX11_DEFAULT);
		loadInfo.Height = (pixelHeight > 0 ? pixelHeight : D3DX11_DEFAULT);
	}

	//continue filling the settings
	loadInfo.Filter = D3DX11_FILTER_LINEAR;
	loadInfo.MiscFlags = D3DX11_DEFAULT;
	loadInfo.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	loadInfo.MiscFlags = D3DX11_DEFAULT;
	loadInfo.CpuAccessFlags = NULL;// cpuAccessFlag;
	loadInfo.Usage =  D3D11_USAGE_DEFAULT;//bufferUsage;


	//create New Texture Object
	HRESULT hr = S_OK;

	//we must check if new name has been used
	if(ValidateUID(texName)== true)
	{
		ERROR_MSG("CreateTextureFromFile : Texture name has been used!!");
		return nullptr;//invalid
	}

	//then endow a pointer to new SRV
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;
	D3DX11CreateShaderResourceViewFromFileA(
		g_pd3dDevice11,
		filePath.c_str(),
		&loadInfo,
		nullptr,
		&tmp_pSRV,
		&hr
		);
		
	//................
	HR_DEBUG_CREATETEX(hr, "CreateTextureFromFile : Create SRV failed ! keepCopyInMem:false");

	//at last create  a new texture object
	ITexture* pTexObj = IFactory<ITexture>::CreateObject(texName);
	std::vector<NVECTOR4> emptyBuff;
	pTexObj->mFunction_InitTexture(tmp_pSRV, texName, std::move(emptyBuff), false, NOISE_TEXTURE_TYPE_COMMON);

	return pTexObj;//invalid file or sth else
}

ITexture* ITextureManager::mFunction_CreateTextureFromFile_KeepACopyInMemory(NFilePath filePath, std::string& texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight)
{
	//!!!!!!!!!!!!!!!!File Size Maybe a problem???

	//check if the file existed
	std::fstream tmpFile(filePath, std::ios::binary | std::ios::in);
	if (tmpFile.bad())
	{
		ERROR_MSG("CreateTextureFromFile : Texture file not found!!");
		return nullptr;//invalid
	}


	//create New Texture Object
	HRESULT hr = S_OK;

	//we must check if new name has been used
	//count() will return 0 if given key dont exists
	if (ValidateUID(texName)== true)
	{
		ERROR_MSG("CreateTextureFromFile : Texture name has been used!!");
		return nullptr;//invalid
	}


#pragma region LoadPixelMatrix


	//some settings about loading image
	D3DX11_IMAGE_LOAD_INFO loadInfo;

	//we must check if user want to use default image size
	if (useDefaultSize)
	{
		loadInfo.Width = D3DX11_DEFAULT;
		loadInfo.Height = D3DX11_DEFAULT;
	}
	else
	{
		loadInfo.Width = (pixelWidth > 0 ? pixelWidth : D3DX11_DEFAULT);
		loadInfo.Height = (pixelHeight > 0 ? pixelHeight : D3DX11_DEFAULT);
	}

	//we must create a texture with  STAGING / DYNAMIC or whatever not default   usage
	loadInfo.Filter = D3DX11_FILTER_LINEAR;
	loadInfo.MiscFlags = D3DX11_DEFAULT;
	loadInfo.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	loadInfo.BindFlags = NULL;// D3D11_BIND_SHADER_RESOURCE;
	loadInfo.MiscFlags = NULL;
	loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ;// cpuAccessFlag;
	loadInfo.Usage = D3D11_USAGE_STAGING;//bufferUsage;

	//create texture from files
	ID3D11Texture2D* pOriginTexture2D;
	D3DX11CreateTextureFromFileA(
		g_pd3dDevice11,
		filePath.c_str(),
		&loadInfo,
		NULL,
		(ID3D11Resource**)&pOriginTexture2D,
		&hr
		);

	HR_DEBUG_CREATETEX(hr, "CreateTexture : Load Texture From File failed! (keep memory copy)");

	//use Map() to get data
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	g_pImmediateContext->Map(pOriginTexture2D, 0, D3D11_MAP_READ, NULL, &mappedSubresource);

	//get picture size info and load pixels to memory
	D3D11_TEXTURE2D_DESC	 originTexDesc;
	pOriginTexture2D->GetDesc(&originTexDesc);

	//I DONT KNOW WHY CreateTextureFromFile()  WILL CHANGE THE SIZE OF TEXTURE !!
	UINT loadedTexWidth = mappedSubresource.RowPitch / NOISE_MACRO_DEFAULT_COLOR_BYTESIZE;//loadedTexDesc.Width;
	UINT loadedTexHeight = originTexDesc.Height;

	//I DONT KNOW WHY CreateTextureFromFile() WILL CHANGE THE SIZE OF TEXTURE !!
	//it seems that the width will be scaled to ..... round off....
	//(it's some kind of optimization that will round off to some value)
	std::vector<NVECTOR4> pixelBuffer(loadedTexWidth*loadedTexHeight);
	pixelBuffer.assign(
		(NVECTOR4*)mappedSubresource.pData,
		(NVECTOR4*)mappedSubresource.pData + loadedTexWidth*loadedTexHeight);



	//remember to Unmap() after every mapping
	g_pImmediateContext->Unmap(pOriginTexture2D, 0);

	ReleaseCOM(pOriginTexture2D);
#pragma endregion LoadPixelMatrix

	
#pragma region CreateTex2D&SRV

	//texture2D desc (create a default usage texture)
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = loadedTexWidth;//determined by the loaded picture
	texDesc.Height = loadedTexHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage =  D3D11_USAGE_DEFAULT;//allow update subresource
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags =NULL;
	texDesc.MiscFlags = NULL;

	//SRV desc
	D3D11_SHADER_RESOURCE_VIEW_DESC SRViewDesc;
	SRViewDesc.Format = texDesc.Format;
	SRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
	SRViewDesc.TextureCube.MostDetailedMip = 0;

	//init data
	D3D11_SUBRESOURCE_DATA texInitDataDesc;
	texInitDataDesc.pSysMem = &pixelBuffer.at(0);
	texInitDataDesc.SysMemPitch = (texDesc.Width) * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE;//bytesize for 1 row
	texInitDataDesc.SysMemSlicePitch = 0;//available for tex3D

	 //create texture2D first 
	ID3D11Texture2D* pTmpTexture2D;
	hr = g_pd3dDevice11->CreateTexture2D(&texDesc, &texInitDataDesc, &pTmpTexture2D);
	if (FAILED(hr))
	{
		ReleaseCOM(pTmpTexture2D);
		ERROR_MSG("CreateTextureFromFile : Create ID3D11Texture2D failed!");
		return nullptr;
	}

	//Create SRV from texture 2D (to a tmp textureObject)
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;
	hr = g_pd3dDevice11->CreateShaderResourceView(pTmpTexture2D, &SRViewDesc, &tmp_pSRV);
	if (FAILED(hr))
	{
		ReleaseCOM(pTmpTexture2D);
		ERROR_MSG("CreateTextureFromFile : Create ID3D11SRV failed!");
		return nullptr;
	}

#pragma endregion CreateTex2D&SRV

	//clear tmp interfaces
	ReleaseCOM(pTmpTexture2D);

	//at last create  a new texture object
	ITexture* pTexObj = IFactory<ITexture>::CreateObject(texName);
	pTexObj->mFunction_InitTexture(tmp_pSRV, texName, std::move(pixelBuffer), true, NOISE_TEXTURE_TYPE_COMMON);

	return pTexObj;
}
