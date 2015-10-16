
/***********************************************************************

							类：NOISE texture Manger

							简述：主要负责管理纹理


************************************************************************/

#include "Noise3D.h"

NoiseTextureManager::NoiseTextureManager()
{
	m_pFatherScene = NULL;
	m_pTextureObjectList = new std::vector<N_Texture_Object>;
};

void	 NoiseTextureManager::SelfDestruction()
{
	for (auto texObj : *m_pTextureObjectList)
	{
		ReleaseCOM(texObj.m_pSRV);
	}
};

UINT NoiseTextureManager::CreateTextureFromFile(LPCWSTR filePath, char* textureName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight)
{
	//!!!!!!!!!!!!!!!!File Size Maybe a problem???

	//check if the file existed
	std::fstream tmpFile(filePath, std::ios::binary | std::ios::in);
	if (tmpFile.bad())
	{
		DEBUG_MSG1("Texture file not found!!");
		return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
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
	loadInfo.CpuAccessFlags = NULL;
	loadInfo.Usage = D3D11_USAGE_DEFAULT;


										 //create New Texture Object
	HRESULT hr = S_OK;
	UINT newTexIndex = m_pTextureObjectList->size();
	N_Texture_Object tmpTexObj;
	std::string tmpString(textureName);

	//we must check if new name has been used
	for (auto t : *m_pTextureObjectList)
	{
		if (t.mTexName == tmpString)
		{
			DEBUG_MSG1("CreateTextureFromFile : Texture name has been used!!");
			return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
		}
	}

	//allocate a new element
	m_pTextureObjectList->push_back(tmpTexObj);


	//then endow a pointer to new SRV

	D3DX11CreateShaderResourceViewFromFile(
		g_pd3dDevice,
		filePath,
		&loadInfo,
		nullptr,
		&m_pTextureObjectList->at(newTexIndex).m_pSRV,
		&hr
		);

	//................
	if (FAILED(hr))
	{
		//delete newly allocated element
		m_pTextureObjectList->pop_back();
		return NOISE_MACRO_INVALID_TEXTURE_ID;
	}

	//endow the tex obj a name
	m_pTextureObjectList->at(newTexIndex).mTexName = tmpString;


	return newTexIndex;//invalid file or sth else
}

UINT NoiseTextureManager::CreateCubeMapFromFiles(LPCWSTR fileName[6], char * cubeTextureName, NOISE_CUBEMAP_SIZE faceSize)
{
	HRESULT hr = S_OK;


#pragma region Load6Pictures
	//create temporary textures , so we should mark the ID to delete it later
	UINT tmpTexID[6];
	std::stringstream tmpTexName;
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
		tmpTexName << "tmpTexture" << i;

		//Create Texture using the slowest usage ( but most flexibility )
		tmpTexID[i] = mFunction_CreateTextureFromFile(
			fileName[i],
			(char*)tmpTexName.str().c_str(),
			FALSE,
			cubeMapWidth,
			cubeMapWidth,
			D3D11_USAGE_DYNAMIC,
			D3D11_CPU_ACCESS_WRITE);

		//one of the texture failed loading
		if (tmpTexID[i] == NOISE_MACRO_INVALID_TEXTURE_ID)
		{
			for (UINT j = 0;j <= i;j++)
			{
				//delete invalid temporary texture
				DeleteTexture(tmpTexID[j]);
			}

			DEBUG_MSG3("NoiseTexMgr :CreateCubeMapFromFiles:create face from file failed ! face ID : ", i, "");
			return NOISE_MACRO_INVALID_TEXTURE_ID;
		}

		tmpTexName.clear();
	}



	//a pixel buffers that can store 6 square textures
	std::vector<NVECTOR4> pixelBuffer[6];
	for (UINT i = 0;i < 6;i++)
	{
		pixelBuffer[i].resize(cubeMapWidth*cubeMapWidth);
	}

	//prepare for MAPPING a resource to mappedResource ( a pointer will be returned to us to use)
	ID3D11Texture2D* tmpTexResource;
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	D3D11_SUBRESOURCE_DATA texInitDataDesc[6];
	ZeroMemory(&mappedSubResource, sizeof(mappedSubResource));


	for (UINT i = 0;i < 6;i++)
	{
		//GetTexture2D (Resource) From SRVs
		m_pTextureObjectList->at(tmpTexID[i]).m_pSRV->GetResource((ID3D11Resource**)&tmpTexResource);

		mappedSubResource.pData = &pixelBuffer[i].at(0);
		mappedSubResource.DepthPitch = 0;
		mappedSubResource.RowPitch = 0;

		//use Map() to get access to resource data (in gpu??)
		hr = g_pImmediateContext->Map(tmpTexResource, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubResource);
		if (FAILED(hr)) 
		{ 
			DEBUG_MSG1(hr);
			DEBUG_MSG1("NoiseTexMgr :CreateCubeMapFromFiles: DeviceContext.Map() Failed"); 
			return NOISE_MACRO_INVALID_TEXTURE_ID;
		}

		//for each picture , copy pixels to corresponding block of pixel buffers
		//totally 6 blocks (6 pictures)
		memcpy(
			&pixelBuffer[i].at(0),
			mappedSubResource.pData,
			cubeMapWidth*cubeMapWidth*NOISE_MACRO_DEFAULT_COLOR_BYTESIZE
			);

		//remember to unmap after done accessing to data
		g_pImmediateContext->Unmap(tmpTexResource, NULL);

		//....
		texInitDataDesc[i].pSysMem = &pixelBuffer[i].at(0);
		texInitDataDesc[i].SysMemPitch = cubeMapWidth * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE;
		texInitDataDesc[i].SysMemSlicePitch = NULL;//only used in texture3D

	}


	//clear rubbish
	ReleaseCOM(tmpTexResource);
	//delete temporary textures after copying data to pixelBuffer(s)
	DeleteTexture(tmpTexID[0]);
	DeleteTexture(tmpTexID[1]);
	DeleteTexture(tmpTexID[2]);
	DeleteTexture(tmpTexID[3]);
	DeleteTexture(tmpTexID[4]);
	DeleteTexture(tmpTexID[5]);

#pragma endregion Load6Pictures


#pragma region CreateCubeMap
	//done retriving pixels of 6 pictures , now create a cube map
	UINT newTexIndex = m_pTextureObjectList->size();
	std::string tmpString(cubeTextureName);

	//we must check if new name has been used
	for (auto t : *m_pTextureObjectList)
	{
		if (t.mTexName == tmpString)
		{
			DEBUG_MSG1("NoiseTexMgr:CreateCubeMapFromFiles : Texture name has been used!!");
			return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
		}
	}


	//texture2D desc
	D3D11_TEXTURE2D_DESC texDesc;
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

	D3D11_SHADER_RESOURCE_VIEW_DESC SRViewDesc;
	SRViewDesc.Format = texDesc.Format;
	SRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SRViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
	SRViewDesc.TextureCube.MostDetailedMip = 0;

	//create texture2D first 
	ID3D11Texture2D* pCubeMapTexture2D;
	hr = g_pd3dDevice->CreateTexture2D(&texDesc, &texInitDataDesc[0], &pCubeMapTexture2D);
	if (FAILED(hr))
	{
		DEBUG_MSG1("NoiseTexMgr:CreateCubeMapFromFiles : Create new cube map failed!!");
		return NOISE_MACRO_INVALID_TEXTURE_ID;
	}

	//...
	N_Texture_Object tmpTexObj;
	m_pTextureObjectList->push_back(tmpTexObj);

	//create SRV from texture2D
	hr = g_pd3dDevice->CreateShaderResourceView(
		pCubeMapTexture2D,
		&SRViewDesc,
		&m_pTextureObjectList->at(newTexIndex).m_pSRV);

	if (FAILED(hr))
	{
		//new temporary tex obj should be dumped
		m_pTextureObjectList->pop_back();
		DEBUG_MSG1("NoiseTexMgr:CreateCubeMapFromFiles : Create new cube map failed!!");
		return NOISE_MACRO_INVALID_TEXTURE_ID;
	}

#pragma endregion CreateCubeMap


	return newTexIndex;
}

UINT NoiseTextureManager::CreateCubeMapFromDDS(LPCWSTR dds_FileName, char * cubeTextureName, NOISE_CUBEMAP_SIZE faceSize)
{
	BOOL isFileNameValid;
	for (UINT i = 0; i < 6;i++)
	{
		//try opening a file
		isFileNameValid = std::fstream(dds_FileName).good();

		//check if  one of the texture id is illegal
		if (!isFileNameValid)
		{
			DEBUG_MSG3("Noise Tex Mgr :CreateCubeTextureFromDDS : file not exist!! ; Index : ", i, ".");
			return NOISE_MACRO_INVALID_TEXTURE_ID;
		}
	}



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
	UINT newTexIndex = m_pTextureObjectList->size();
	N_Texture_Object tmpTexObj;
	std::string tmpString(cubeTextureName);

	//we must check if new name has been used
	for (auto t : *m_pTextureObjectList)
	{
		if (t.mTexName == tmpString)
		{
			DEBUG_MSG1("CreateCubeMapFromDDS : Texture name has been used!!");
			return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
		}
	}

	//allocate a new element
	m_pTextureObjectList->push_back(tmpTexObj);


	//then endow a pointer to new SRV
	D3DX11CreateShaderResourceViewFromFile(
		g_pd3dDevice,
		dds_FileName,
		&loadInfo,
		nullptr,
		&m_pTextureObjectList->at(newTexIndex).m_pSRV,
		&hr
		);

	//................
	if (FAILED(hr))
	{
		//delete newly allocated element
		m_pTextureObjectList->pop_back();
		return NOISE_MACRO_INVALID_TEXTURE_ID;
	}

	//endow the tex obj a name
	m_pTextureObjectList->at(newTexIndex).mTexName = tmpString;

	return newTexIndex;
}


UINT	 NoiseTextureManager::GetIndexByName(char* textureName)
{
	N_Texture_Object tmpTexObj;
	std::string tmpString(textureName);

	for (UINT i = 0;i < m_pTextureObjectList->size();i++)
	{
		tmpTexObj = m_pTextureObjectList->at(i);

		//if find specified tex name
		if (tmpTexObj.mTexName == tmpString)
		{
			//return corresponding index
			return i;
		}
	}

	return NOISE_MACRO_INVALID_TEXTURE_ID;//Invalid texture Name
};

void	 NoiseTextureManager::GetNameByIndex(UINT index, std::string* outTextureName)
{
	//if index is valid, return an std::string name
	if (index < m_pTextureObjectList->size())
	{
		*outTextureName = m_pTextureObjectList->at(index).mTexName;
	}
}

UINT	 NoiseTextureManager::GetTextureCount()
{
	return m_pTextureObjectList->size();
}

BOOL NoiseTextureManager::DeleteTexture(UINT texID)
{
	auto iter = m_pTextureObjectList->begin();
	if (texID < m_pTextureObjectList->size())
	{
		iter += texID;
		//safe_release SRV  interface
		ReleaseCOM(m_pTextureObjectList->at(texID).m_pSRV);
		m_pTextureObjectList->erase(iter);
	}
	else
	{
		DEBUG_MSG1("NoiseTexMgr: DeleteTexture : texID invalid!!");
		return FALSE;
	}

	return TRUE;
}



/*************************************************************
							P R I V A T E
*************************************************************/


UINT	 NoiseTextureManager::mFunction_ValidateTextureID(UINT texID, NOISE_TEXTURE_TYPE texType)
{

	//if texID is invalid , an invalid flag should be returned
	if (texID == NOISE_MACRO_INVALID_TEXTURE_ID || texID >= m_pTextureObjectList->size())
	{
		return NOISE_MACRO_INVALID_TEXTURE_ID;
	}



	//then check the texture type : common / cubemap / volumn
	D3D11_SHADER_RESOURCE_VIEW_DESC tmpSRViewDesc;
	m_pTextureObjectList->at(texID).m_pSRV->GetDesc(&tmpSRViewDesc);



	switch (tmpSRViewDesc.ViewDimension)
	{
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		if (texType != NOISE_TEXTURE_TYPE_CUBEMAP)
		{
			return NOISE_MACRO_INVALID_TEXTURE_ID;
		}
		break;

	case D3D11_SRV_DIMENSION_TEXTURE3D:
		if (texType != NOISE_TEXTURE_TYPE_VOLUMN)
		{
			return NOISE_MACRO_INVALID_TEXTURE_ID;
		}
		break;

	case D3D11_SRV_DIMENSION_TEXTURE2D:
		if (texType != NOISE_TEXTURE_TYPE_COMMON)
		{
			return NOISE_MACRO_INVALID_TEXTURE_ID;
		}
		break;

	}


	//a no-problem texID
	return texID;
}

UINT NoiseTextureManager::mFunction_CreateTextureFromFile(LPCWSTR filePath, char * textureName, BOOL useDefaultSize, UINT pixelWidth, UINT pixelHeight, D3D11_USAGE bufferUsage, UINT cpuAccessFlag)
{
	//!!!!!!!!!!!!!!!!File Size Maybe a problem???

	//check if the file existed
	std::fstream tmpFile(filePath, std::ios::binary | std::ios::in);
	if (tmpFile.bad())
	{
		DEBUG_MSG1("Texture file not found!!");
		return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
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
	loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE;// cpuAccessFlag;
	loadInfo.Usage = D3D11_USAGE_DYNAMIC;//bufferUsage;


	//create New Texture Object
	HRESULT hr = S_OK;
	UINT newTexIndex = m_pTextureObjectList->size();
	N_Texture_Object tmpTexObj;
	std::string tmpString(textureName);

	//we must check if new name has been used
	for (auto t : *m_pTextureObjectList)
	{
		if (t.mTexName == tmpString)
		{
			DEBUG_MSG1("CreateTextureFromFile : Texture name has been used!!");
			return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
		}
	}

	//allocate a new element
	m_pTextureObjectList->push_back(tmpTexObj);


	//then endow a pointer to new SRV

		D3DX11CreateShaderResourceViewFromFile(
		g_pd3dDevice,
		filePath,
		&loadInfo,
		nullptr,
		&m_pTextureObjectList->at(newTexIndex).m_pSRV,
		&hr
		);
		
	//................
	if (FAILED(hr))
	{
		//delete newly allocated element
		m_pTextureObjectList->pop_back();
		return NOISE_MACRO_INVALID_TEXTURE_ID;
	}

	//endow the tex obj a name
	m_pTextureObjectList->at(newTexIndex).mTexName = tmpString;


	return newTexIndex;//invalid file or sth else
};





