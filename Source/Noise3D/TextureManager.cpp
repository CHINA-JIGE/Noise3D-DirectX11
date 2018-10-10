
/***********************************************************************

							class£ºNOISE texture Manger

							texture management/modification

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

TextureManager::TextureManager():
	IFactory<Texture2D>(100000),//maxCount of Textures,
	IFactory<TextureCubeMap>(100000)
{

};

TextureManager::~TextureManager()
{
	DeleteAllTexture();
}


//--------------------------------TEXTURE CREATION-----------------------------
Texture2D* TextureManager::CreatePureColorTexture(N_UID texName, UINT pixelWidth, UINT pixelHeight, NVECTOR4 color, bool keepCopyInMemory)
{
	return TextureManager::CreatePureColorTexture(texName, pixelWidth, pixelHeight, NColor4u(color), keepCopyInMemory);
};

Texture2D * Noise3D::TextureManager::CreatePureColorTexture(N_UID texName, UINT pixelWidth, UINT pixelHeight, NColor4u color, bool keepCopyInMemory)
{
	//create New Texture Object
	HRESULT hr = S_OK;

	//we must check if new name has been used
	if(TextureManager::ValidateTex2D(texName)== true)
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
	std::vector<NColor4u> initPixelBuffer(pixelWidth*pixelHeight, color);


	//texture2D desc (create a default usage texture)
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = pixelWidth;//determined by the loaded picture
	texDesc.Height = pixelHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = c_DefaultPixelDxgiFormat;// DXGI_FORMAT_R32G32B32A32_FLOAT;
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
	Texture2D* pTexObj= IFactory<Texture2D>::CreateObject(texName);
	pTexObj->mFunction_InitTexture(tmp_pSRV, texName, std::move(initPixelBuffer), keepCopyInMemory);

	return pTexObj;
};

Texture2D* TextureManager::CreateTextureFromFile(NFilePath filePath, N_UID texName, bool useDefaultSize, UINT pixelWidth, UINT pixelHeight,bool keepCopyInMemory)
{
	//read file to memory
	IFileIO fileIO;
	std::vector<char> fileBuff;
	bool isReadFileSucceeded = fileIO.ImportFile_PURE(filePath, fileBuff);
	if (!isReadFileSucceeded)
	{
		ERROR_MSG("CreateTextureFromFile : failed to read file!!");
		return nullptr;//invalid
	}

	//check if new name has been used
	//count() will return 0 if given key dont exists
	if (TextureManager::ValidateTex2D(texName) == true)
	{
		ERROR_MSG("CreateTextureFromFile : Texture name has been used!!");
		return nullptr;//invalid
	}

#pragma region DirectXTex load Image

	//enumerate file formats, and parse image file using different functions
	std::string fileSubfix = Ut::GetFileSubFixFromPath(filePath);
	NOISE_IMAGE_FILE_FORMAT fileFormat = mFunction_GetImageFileFormat(fileSubfix);
	DirectX::TexMetadata srcMetaData;//meta data is loaded from target image file
	DirectX::ScratchImage srcImage;

	switch (fileFormat)
	{
		//Supported by DirectXTex.WIC
		case NOISE_IMAGE_FILE_FORMAT_BMP:
		case NOISE_IMAGE_FILE_FORMAT_JPG:
		case NOISE_IMAGE_FILE_FORMAT_PNG:
		case NOISE_IMAGE_FILE_FORMAT_TIFF:
		case NOISE_IMAGE_FILE_FORMAT_GIF:
		{
			DirectX::LoadFromWICMemory(&fileBuff.at(0), fileBuff.size(), DirectX::WIC_FLAGS_NONE, &srcMetaData, srcImage);
			break;
		}

		case NOISE_IMAGE_FILE_FORMAT_HDR:
		{
			DirectX::LoadFromHDRMemory(&fileBuff.at(0), fileBuff.size(), &srcMetaData, srcImage);
			break;
		}

		case NOISE_IMAGE_FILE_FORMAT_TGA:
		{
			DirectX::LoadFromTGAMemory(&fileBuff.at(0), fileBuff.size(), &srcMetaData, srcImage);
			break;
		}

		case NOISE_IMAGE_FILE_FORMAT_DDS :
		{
			DirectX::LoadFromDDSMemory(&fileBuff.at(0), fileBuff.size(),DirectX::DDS_FLAGS_NONE , &srcMetaData, srcImage);
			break;
		}

		case NOISE_IMAGE_FILE_FORMAT_NOT_SUPPORTED:
		default:
			ERROR_MSG("CreateTextureFromFile : image file format not supported!! format:"+ fileSubfix);
			return nullptr;//invalid
			break;
	}

	//load and resize image data to a memory block
	uint32_t resizedImageWidth = useDefaultSize ? srcMetaData.width : pixelWidth;
	uint32_t resizedImageHeight = useDefaultSize ? srcMetaData.height : pixelHeight;

	//re-sampling of original images
	DirectX::ScratchImage resizedImage;
	DirectX::ScratchImage convertedImage;

	//resize the image
	DirectX::Resize(
		srcImage.GetImages(), 
		srcImage.GetImageCount(),
		srcImage.GetMetadata(), 
		resizedImageWidth, 
		resizedImageHeight,
		DirectX::TEX_FILTER_DEFAULT,//DirectX::TEX_FILTER_LINEAR,
		resizedImage);

	//pixel format conversion (this format must match the format defined in ID3D11Texture2D desc)
	DXGI_FORMAT format = resizedImage.GetMetadata().format;
	//special care for BlockCompression(BC) format, DirectX::Convert can't directly apply to BC images
	if (mFunction_IsBlockCompressionFormat(format))
	{
		DirectX::Decompress(
			resizedImage.GetImages(),
			resizedImage.GetImageCount(),
			resizedImage.GetMetadata(),
			c_DefaultPixelDxgiFormat,
			convertedImage);
	}
	else if (format != c_DefaultPixelDxgiFormat)
	{
		DirectX::Convert(
			resizedImage.GetImages(),
			resizedImage.GetImageCount(),
			resizedImage.GetMetadata(),
			c_DefaultPixelDxgiFormat,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			convertedImage);
	}
	else
	{
		//copy
		convertedImage.InitializeFromImage(*resizedImage.GetImages());
	}

#pragma endregion DirectXTex load Image

#pragma region CreateTexture2DArray & SRV

	//texture2D desc (create a default usage texture)
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = resizedImageWidth;//determined by the loaded picture
	texDesc.Height = resizedImageHeight;
	texDesc.MipLevels = srcMetaData.mipLevels;// 1;
	texDesc.ArraySize = srcMetaData.arraySize;//should be 6
	texDesc.Format = c_DefaultPixelDxgiFormat;
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
	texInitDataDesc.pSysMem = convertedImage.GetPixels() ;// &pixelBuffer.at(0);
	texInitDataDesc.SysMemPitch = (texDesc.Width) * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE;//bytesize for 1 row
	texInitDataDesc.SysMemSlicePitch = 0;//available for tex3D

	//create ID3D11Texture2D first 
	ID3D11Texture2D* pTmpTexture2D;
	HRESULT hr = g_pd3dDevice11->CreateTexture2D(&texDesc, &texInitDataDesc, &pTmpTexture2D);
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

#pragma endregion CreateTexture2DArray & SRV

	//clear tmp interfaces
	ReleaseCOM(pTmpTexture2D);

	//at last, create a new texture object ptr
	Texture2D* pTexObj = IFactory<Texture2D>::CreateObject(texName);
	if (keepCopyInMemory)
	{
		uint32_t pixelCount = resizedImageWidth * resizedImageHeight;
		uint8_t* pData = convertedImage.GetPixels();
		std::vector<NColor4u> pixelBuffer(pixelCount);

		//copy data of converted image to ITexture's system memory
		for (uint32_t pixelId = 0; pixelId < pixelCount; ++pixelId)
		{
			pixelBuffer[pixelId] = *(NColor4u*)(pData + pixelId * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE);
		}

		pTexObj->mFunction_InitTexture(tmp_pSRV, texName, std::move(pixelBuffer), true);
	}
	else
	{
		std::vector<NColor4u> emptyBuff;
		pTexObj->mFunction_InitTexture(tmp_pSRV, texName, std::move(emptyBuff), false);
	}

	return pTexObj;//invalid file or sth else
}

TextureCubeMap* TextureManager::CreateCubeMapFromDDS(NFilePath dds_FileName, N_UID cubeTextureName, bool keepCopyInMemory)
{
	//read file to memory
	IFileIO fileIO;
	std::vector<char> fileBuff;
	bool isReadFileSucceeded = fileIO.ImportFile_PURE(dds_FileName, fileBuff);
	if (!isReadFileSucceeded)
	{
		ERROR_MSG("CreateTextureFromFile : failed to read file!!");
		return nullptr;//invalid
	}

	//check if new name has been used
	//count() will return 0 if given key dont exists
	if (TextureManager::ValidateTexCubeMap(cubeTextureName))
	{
		ERROR_MSG("CreateTextureFromFile : Texture name has been used!!");
		return nullptr;//invalid
	}

#pragma region DirectXTex load Image

	//enumerate file formats, and parse image file using different functions
	std::string fileSubfix = Ut::GetFileSubFixFromPath(dds_FileName);
	NOISE_IMAGE_FILE_FORMAT fileFormat = mFunction_GetImageFileFormat(fileSubfix);
	if (fileFormat != NOISE_IMAGE_FILE_FORMAT_DDS)
	{
		ERROR_MSG("CreateCubeMapFromDDS : the input is not .dds file!!");
		return nullptr;//invalid
	}

	//load image
	DirectX::TexMetadata srcMetaData;//meta data is loaded from target image file
	DirectX::ScratchImage srcImage;
	DirectX::LoadFromDDSMemory(&fileBuff.at(0), fileBuff.size(), DirectX::DDS_FLAGS_NONE, &srcMetaData, srcImage);

	//array size should be 6!!
	if (!srcMetaData.IsCubemap())
	{
		ERROR_MSG("CreateCubeMapFromDDS: this is not an valid cube map! cube map should have 6 faces!!");
		return nullptr;
	}

	//convert pixel format
	DirectX::ScratchImage convertedImage;
	
	//pixel format conversion (this format must match the format defined in ID3D11Texture2D desc)
	DXGI_FORMAT format = srcImage.GetMetadata().format;
	//special care for BlockCompression(BC) format, DirectX::Convert can't directly apply to BC images
	if (mFunction_IsBlockCompressionFormat(format))
	{
		DirectX::Decompress(
			srcImage.GetImages(),
			srcImage.GetImageCount(),
			srcImage.GetMetadata(),
			c_DefaultPixelDxgiFormat,
			convertedImage);
	}
	else if (format != c_DefaultPixelDxgiFormat)
	{
		DirectX::Convert(
			srcImage.GetImages(),
			srcImage.GetImageCount(),
			srcImage.GetMetadata(),
			c_DefaultPixelDxgiFormat,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			convertedImage);
	}
	else
	{
		//copy
		convertedImage.InitializeArrayFromImages(srcImage.GetImages(), srcImage.GetImageCount());
	}

#pragma endregion DirectXTex load Image

#pragma region CreateSRV

	//then assign a pointer to new SRV
	ID3D11ShaderResourceView* tmp_pSRV = nullptr;
	HRESULT hr = DirectX::CreateShaderResourceView(
		g_pd3dDevice11,
		srcImage.GetImages(),
		srcImage.GetImageCount(),
		srcMetaData,
		&tmp_pSRV);

	//................
	if (FAILED(hr))
	{
		ERROR_MSG("CreateCubeMapFromDDS : Create SRV failed!");
		return nullptr;
	}

#pragma endregion CreateSRV

	//Create a new Texture object
	TextureCubeMap* pTexObj = IFactory<TextureCubeMap>::CreateObject(cubeTextureName);
	//std::vector<NColor4u> emptyBuff;
	//pTexObj->mFunction_InitTexture(tmp_pSRV, cubeTextureName, std::move(emptyBuff), false);
	if (keepCopyInMemory)
	{
		uint32_t pixelCount = srcMetaData.width*srcMetaData.height * srcMetaData.arraySize;//arraysize should be 6
		uint8_t* pData = convertedImage.GetPixels();
		std::vector<NColor4u> pixelBuffer(pixelCount);

		//copy data of converted image to ITexture's system memory
		for (uint32_t pixelId = 0; pixelId<pixelCount; ++pixelId)
		{
			pixelBuffer[pixelId] = *(NColor4u*)(pData + pixelId * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE);
		}

		pTexObj->mFunction_InitTexture(tmp_pSRV, cubeTextureName, std::move(pixelBuffer), true);
	}
	else
	{
		std::vector<NColor4u> emptyBuff;
		pTexObj->mFunction_InitTexture(tmp_pSRV, cubeTextureName, std::move(emptyBuff), false);
	}

	//return new texObj ptr
	return pTexObj;
}

Texture2D * TextureManager::GetTexture2D(N_UID texName)
{
	return IFactory<Texture2D>::GetObjectPtr(texName);
}

TextureCubeMap * TextureManager::GetTextureCubeMap(N_UID texName)
{
	return IFactory<TextureCubeMap>::GetObjectPtr(texName);
}

uint32_t	 TextureManager::GetTexture2DCount()
{
	return IFactory<Texture2D>::GetObjectCount();
}

uint32_t Noise3D::TextureManager::GetTextureCubeMapCount()
{
	return IFactory<TextureCubeMap>::GetObjectCount();
}

bool TextureManager::DeleteTexture(Texture2D * pTex)
{
	if (pTex == nullptr)
	{
		return false;
	}
	else
	{
		return IFactory<Texture2D>::DestroyObject(pTex->GetTextureName());
	}
}

bool Noise3D::TextureManager::DeleteTexture(TextureCubeMap * pTex)
{
	if (pTex == nullptr)
	{
		return false;
	}
	else
	{
		return IFactory<TextureCubeMap>::DestroyObject(pTex->GetTextureName());
	}
}

bool TextureManager::DeleteTexture2D(N_UID uid)
{
	return IFactory<Texture2D>::DestroyObject(uid);
}

bool Noise3D::TextureManager::DeleteTextureCubeMap(N_UID uid)
{
	return IFactory<TextureCubeMap>::DestroyObject(uid);
}

void TextureManager::DeleteAllTexture()
{
	IFactory<Texture2D>::DestroyAllObject();
	IFactory<TextureCubeMap>::DestroyAllObject();
}

bool Noise3D::TextureManager::ValidateTex2D(N_UID texName)
{
	return IFactory<Texture2D>::FindUid(texName);
}

bool Noise3D::TextureManager::ValidateTexCubeMap(N_UID texName)
{
	return IFactory<TextureCubeMap>::FindUid(texName);
}

/*************************************************************
									P R I V A T E
*************************************************************/
NOISE_IMAGE_FILE_FORMAT Noise3D::TextureManager::mFunction_GetImageFileFormat(const std::string & fileSubfix)
{
	std::string lowCaseSubfix;
	for (auto c : fileSubfix)lowCaseSubfix.push_back(::tolower(c));

	if (lowCaseSubfix == "bmp" )return NOISE_IMAGE_FILE_FORMAT_BMP;
	if (lowCaseSubfix == "png")return NOISE_IMAGE_FILE_FORMAT_PNG;
	if (lowCaseSubfix == "jpg")return NOISE_IMAGE_FILE_FORMAT_JPG;
	if (lowCaseSubfix == "tiff")return NOISE_IMAGE_FILE_FORMAT_TIFF;
	if (lowCaseSubfix == "gif")return NOISE_IMAGE_FILE_FORMAT_GIF;
	if (lowCaseSubfix == "tga")return NOISE_IMAGE_FILE_FORMAT_TGA;
	if (lowCaseSubfix == "hdr")return NOISE_IMAGE_FILE_FORMAT_HDR;
	if (lowCaseSubfix == "dds")return NOISE_IMAGE_FILE_FORMAT_DDS;
	return NOISE_IMAGE_FILE_FORMAT_NOT_SUPPORTED;
}

bool Noise3D::TextureManager::mFunction_IsBlockCompressionFormat(DXGI_FORMAT f)
{
	return ((f>=DXGI_FORMAT_BC1_TYPELESS) && (f<=DXGI_FORMAT_BC5_UNORM))|| 
		((f>=DXGI_FORMAT_BC6H_TYPELESS) &&(f<=DXGI_FORMAT_BC7_UNORM_SRGB)) ;
}
