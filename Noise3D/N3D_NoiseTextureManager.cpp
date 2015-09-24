
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
	if(FAILED(hr))
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
	BOOL isFileNameValid;
	for (UINT i = 0; i < 6;i++)
	{
		//try opening a file
		isFileNameValid = std::fstream(fileName[i]).good();

		//check if  one of the texture id is illegal
		if (!isFileNameValid)
		{
			DEBUG_MSG3("Noise Tex Mgr :CreateCubeTexture : file not exist!! ; Index : ", i, ".");
			return NOISE_MACRO_INVALID_TEXTURE_ID;
		}
	}




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
			DEBUG_MSG1("CreateTextureFromFile : Texture name has been used!!");
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
};




