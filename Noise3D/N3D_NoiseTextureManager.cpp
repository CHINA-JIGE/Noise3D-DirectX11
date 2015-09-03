
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

void NoiseTextureManager::SelfDestruction()
{
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
			DEBUG_MSG1("Texture name has been used!!");
			return NOISE_MACRO_INVALID_TEXTURE_ID;//invalid
		}
	}

	//allocate a new element
	m_pTextureObjectList->push_back(tmpTexObj);

	//Create SRV
	D3DX11CreateShaderResourceViewFromFile(
		g_pd3dDevice,
		filePath,
		&loadInfo,
		NULL,
		&m_pTextureObjectList->at(newTexIndex).m_pSRV,
		&hr
		);
	HR_DEBUG(hr, "Create SRV failed");

	//endow the tex obj a name
	m_pTextureObjectList->at(newTexIndex).mTexName = tmpString;


	return newTexIndex;//invalid file or sth else
};

UINT NoiseTextureManager::GetIndexByName(char* textureName)
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

void NoiseTextureManager::GetNameByIndex(UINT index, std::string* outTextureName)
{
	//if index is valid, return an std::string name
	if (index < m_pTextureObjectList->size())
	{
		*outTextureName = m_pTextureObjectList->at(index).mTexName;
	}
}

UINT NoiseTextureManager::GetTextureCount()
{
	return m_pTextureObjectList->size();
};


