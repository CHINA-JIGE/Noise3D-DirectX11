
/***********************************************************************

                ITexture, interface for common 2d texture

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

Texture2D::Texture2D()
{

}

Texture2D::~Texture2D()
{
}

void Texture2D::SetPixel(UINT x, UINT y, const Color4u & color)
{
	if (mIsPixelBufferInMemValid)
	{
		if (x >= 0 && x < mWidth && y >= 0 && y < mHeight)
		{
			UINT pixelIndex = y*mWidth + x;
			mPixelBuffer.at(pixelIndex) = color;
		}
		else
		{
			ERROR_MSG("ITexture::SetPixel : Point out of range!");
		}

	}
	else
	{
		ERROR_MSG("ITexture::SetPixel : Texture ID or Texture Type invalid !!");
	}
}

Color4u Texture2D::GetPixel(UINT x, UINT y)const
{
	if (ITexture::IsSysMemBufferValid())
	{
		if (x >= 0 && x < mWidth && y >= 0 && y < mHeight)
		{
			UINT pixelIndex = y*mWidth + x;
			return mPixelBuffer.at(pixelIndex);
		}
		else
		{
			WARNING_MSG("GetPixel : Point out of range!");
		}
	}
	else
	{
		WARNING_MSG("GetPixel : didn't keep a copy in memory !!!");
	}
	return Color4u(0, 0, 0, 0);
}

//less redundant bound check to increase efficiency
bool Texture2D::SetPixelArray(const std::vector<Color4u>& in_ColorArray)
{
	if (ITexture::IsSysMemBufferValid())
	{
		//check if the array size matches
		if (in_ColorArray.size() == mPixelBuffer.size())
		{
			mPixelBuffer.assign(in_ColorArray.begin(), in_ColorArray.end());
			return true;
		}
		else
		{
			ERROR_MSG("SetPixelArray : array size didn't match.");
		}
	}
	return false;
}

bool Texture2D::SetPixelArray(std::vector<Color4u>&& in_ColorArray)
{
	if (ITexture::IsSysMemBufferValid())
	{
		//check if the array size matches
		if (in_ColorArray.size() == mPixelBuffer.size())
		{
			mPixelBuffer = std::move(in_ColorArray);
			return true;
		}
		else
		{
			ERROR_MSG("SetPixelArray : array size didn't match.");
		}
	}
	return false;
}

bool Texture2D::GetPixelArray(std::vector<Color4u>& outColorArray) const
{
	if (ITexture::IsSysMemBufferValid())
	{
		outColorArray = mPixelBuffer;
		return true;
	}
	else
	{
		return false;
	}
}

Color4f Noise3D::Texture2D::SamplePixelBilinear(Vec2 texcoord) const
{
	if (ITexture::IsSysMemBufferValid())
	{
		float px_f = texcoord.x * float(mWidth);
		float py_f = texcoord.y * float(mHeight);
		uint32_t px = uint32_t(px_f);
		uint32_t py = uint32_t(py_f);

		uint32_t pixelId[4] =
		{
			((py) % mHeight) *mWidth + ((px) % mWidth),
			((py+1) % mHeight) *mWidth + ((px) % mWidth),
			((py) % mHeight) *mWidth + ((px+1) % mWidth),
			((py+1) % mHeight) *mWidth + ((px+1) % mWidth)
		};

		Color4f pixels[4];
		for (int i = 0; i < 4; ++i)
		{
			const Color4u& c = mPixelBuffer.at(pixelId[i]);
			pixels[i] = (c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
		}

		//bilinear interpolation's uv within these 4 pixels
		float local_u = px_f - float(px);
		float local_v = py_f - float(py);

		Color4f tmp1 = XMVectorLerp(pixels[0],pixels[2],local_u) ;
		Color4f tmp2 = XMVectorLerp(pixels[1], pixels[3], local_u);
		Color4f result = XMVectorLerp(tmp1, tmp2, local_v);
		return result;
	}
	else
	{
		WARNING_MSG("GetPixel : didn't keep a copy in memory !!!");
	}
	return Color4f(0, 0, 0, 0);
}

//if user modified pixels via setPixel()/setPixelArray(), 
//then UpdateToVideoMem() should be called after modification.
bool Texture2D::UpdateToVideoMemory()
{
	if (ITexture::IsSysMemBufferValid())
	{
		//after modifying buffer in memory, update to GPU
		ID3D11Resource* pTmpRes;
		//resource reference count will increase ,so remember to release
		//so getResource() actually gets a interface to the resource BOUND to the view.
		m_pSRV->GetResource(&pTmpRes);

		//update resource which is bound to the corresponding SRV
		g_pImmediateContext->UpdateSubresource(
			pTmpRes,
			0,
			NULL,
			&mPixelBuffer.at(0),
			mWidth * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE,
			NULL);

		ReleaseCOM(pTmpRes);
	}
	else
	{
		//mIsPixelBufferInMemValid==false
		ERROR_MSG("UpdateTextureToVideoMemory : Texture didn't have a copy in System Memory!");
		return false;
	}

	return true;
}

bool Texture2D::ConvertTextureToGreyMap()
{
	return ConvertTextureToGreyMap(0.3f, 0.59f, 0.1f);
}

bool Texture2D::ConvertTextureToGreyMap(float factorR, float factorG, float factorB)
{
	//only the texture created both in gpu & memory can be modified 
	//( actually the sysMem block will be modified)
	if (!ITexture::IsSysMemBufferValid())
	{
		ERROR_MSG("ConvertTextureToGreyMap:Only Textures that keep a copy in memory can be converted ! ");
		return false;
	}

	//Get Dimension info
	/*D3D11_TEXTURE2D_DESC textureDesc;
	ID3D11Texture2D* pTex2D;
	m_pSRV->GetResource((ID3D11Resource**)&pTex2D);
	pTex2D->GetDesc(&textureDesc);*/

	UINT picWidth = GetWidth();//textureDesc.Width;
	UINT picHeight = GetWidth();//textureDesc.Height;

	//ReleaseCOM(pTex2D);

	//for every pixel of the copy in memory ,
	//convert their RGBs to same value under some rules
	for (auto& c : mPixelBuffer)
	{
		//float greyScale = factorR *c.x + factorG*c.y + factorB*c.z;
		float greyScale = factorR *c.r + factorG*c.g + factorB*c.b;
		c = Color4u(uint8_t(greyScale), uint8_t(greyScale), uint8_t(greyScale), c.a);
	}

	//after modifying buffer in memory, update to GPU
	ID3D11Resource* pTmpRes;
	//resource reference count will increase ,so remember to release
	//so getResource() actually gets a interface to the resource BOUND to the view.
	m_pSRV->GetResource(&pTmpRes);

	//update resource which is bound to the corresponding SRV
	g_pImmediateContext->UpdateSubresource(
		pTmpRes,
		0,
		NULL,
		&mPixelBuffer.at(0),
		picWidth * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE,
		NULL);

	ReleaseCOM(pTmpRes);

	return false;
}

bool Texture2D::ConvertHeightMapToNormalMap(float heightFieldScaleFactor)
{
	//only the texture created both in gpu & memory can be modified 
	//( actually the copy in mem will be modified)
	if (!ITexture::IsSysMemBufferValid())
	{
		ERROR_MSG("ITexture::ConvertTextureToNormalMap:Only Textures that keep a copy in memory can be converted! ");
		return false;
	}

	//texture size might be modified when being transformed into D3D11 resource
	UINT picWidth = GetWidth();
	UINT picHeight = GetHeight();

	//every pixel of the height field stands for a 3D point
	//and we can use cross product to generate a pixel matrix of (n-1)x(n-1) dimension
	//the last 1 row & column need to be dealt with specially

	auto ComputeIndex = [picWidth](UINT x, UINT y)->UINT
	{
		return y*picWidth + x;
	};


	//use a temp buffer to avoid calculated pixel from being affected by previous normal vectors' color;
	std::vector<Color4u> tmpNormalMap(mPixelBuffer.size());
	//loop to generate normal map
	for (UINT j = 0;j < picHeight;j++)
	{
		for (UINT i = 0;i < picWidth;i++)
		{
			Vec3	currentNormal(0, 0, 0);

			UINT vertexID1=0, vertexID2=0, vertexID3=0;

			//the LSB to indicate if (j==height-1), the 2nd LSB indicate if i==(width-1)
			UINT caseID = (((i == (picWidth-1))) << 1) | (j ==( picHeight-1));
			switch (caseID)
			{
			default:
			case 0://common situation
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(i + 1, j);//i+1,j
				vertexID3 = ComputeIndex(i, j + 1);//i, j+1
				break;
				}

			case 1://the most bottom row(j==height-1)
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(i+1,j);//i+1,j
				vertexID3 = ComputeIndex(i, 0);//i, j+1
				break;
				}

			case 2://the most right column (i==width-1)
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(0, j);//i+1,j
				vertexID3 = ComputeIndex(i, j + 1);//i, j+1
				break;
				}

			case 3://right bottom corner pixel
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(0, j);//i+1,j
				vertexID3 = ComputeIndex(i, 0);//i, j+1
				break;
				}

			}

			//after confirm 3 vertices composing a triangle, apply CROSS operation
			Color4u color1 = mPixelBuffer.at(vertexID1);
			Color4u color2 = mPixelBuffer.at(vertexID2);
			Color4u color3 = mPixelBuffer.at(vertexID3);
			//because it's grey map , so we can only use one color channel
			Vec3	v1 = Vec3(1.0f, 0, heightFieldScaleFactor* (color2.r - color1.r));
			Vec3	v2 = Vec3(0, 1.0f, heightFieldScaleFactor* (color3.r - color1.r));
			currentNormal = v1.Cross(v2);
			//D3DXVec3Cross(&currentNormal, &v1, &v2);

			//convert normal to Normal Map Color
			tmpNormalMap.at(vertexID1) =
				Color4u(Vec4((currentNormal.x + 1.0f) / 2.0f, (currentNormal.y + 1.0f) / 2.0f, (currentNormal.z + 1.0f) / 2.0f, 1.0f));
		}
	}

	//copy calculated result
	mPixelBuffer = std::move(tmpNormalMap);

	//after modifying buffer in memory, update to GPU
	ID3D11Resource* pTmpRes;
	//resource reference count will increase ,so remember to release
	//so getResource() actually gets a interface to the resource BOUND to the view.
	m_pSRV->GetResource(&pTmpRes);

	//update resource which is bound to the corresponding SRV
	g_pImmediateContext->UpdateSubresource(
		pTmpRes,
		0,
		NULL,
		&mPixelBuffer.at(0),
		picWidth * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE,
		NULL);

	ReleaseCOM(pTmpRes);

	return true;
}

bool Texture2D::SaveTexture2DToFile(NFilePath filePath, NOISE_IMAGE_FILE_FORMAT picFormat)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* pSrcTexture;
	m_pSRV->GetResource((ID3D11Resource**)&pSrcTexture);

	//create a temp texture that can be Map() and access by CPU
	D3D11_TEXTURE2D_DESC tmpTexDesc;
	ZeroMemory(&tmpTexDesc, sizeof(tmpTexDesc));
	pSrcTexture->GetDesc(&tmpTexDesc);
	tmpTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tmpTexDesc.Usage = D3D11_USAGE_STAGING;
	tmpTexDesc.BindFlags = 0;
	tmpTexDesc.MiscFlags = 0;

	ID3D11Texture2D* pCpuReadTexture = nullptr;
	if (FAILED(g_pd3dDevice11->CreateTexture2D(&tmpTexDesc, nullptr, &pCpuReadTexture)))
	{
		ERROR_MSG("ITexture: failed to save texture. (failed to create intermediate texture2D.)");
		return false;
	}
	else
	{
		g_pImmediateContext->CopyResource(pCpuReadTexture, pSrcTexture);
	}

	//then use Map() to get data
	DirectX::Blob outFileData;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_pImmediateContext->Map(pCpuReadTexture, 0, D3D11_MAP_READ, 0, &mappedResource);


	DirectX::Image outImage;
	outImage.pixels = (uint8_t*)mappedResource.pData;
	outImage.format = tmpTexDesc.Format;
	outImage.width = tmpTexDesc.Width;
	outImage.height = tmpTexDesc.Height;
	outImage.rowPitch = mappedResource.RowPitch;
	outImage.slicePitch = mappedResource.DepthPitch;

	//save texture to DirectX::Blob encoded in specific format
	switch (picFormat)
	{
		//Supported by DirectXTex.WIC
	case NOISE_IMAGE_FILE_FORMAT_BMP:
		DirectX::SaveToWICMemory(outImage, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_BMP), outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_JPG:
		DirectX::SaveToWICMemory(outImage, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_JPEG), outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_PNG:
		DirectX::SaveToWICMemory(outImage, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_PNG), outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_TIFF:
		DirectX::SaveToWICMemory(outImage, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_TIFF), outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_GIF:
		DirectX::SaveToWICMemory(outImage, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(DirectX::WIC_CODEC_GIF), outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_HDR:
		DirectX::SaveToHDRMemory(outImage, outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_TGA:
		DirectX::SaveToTGAMemory(outImage, outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_DDS:
		DirectX::SaveToDDSMemory(outImage, DirectX::DDS_FLAGS_NONE, outFileData);
		break;

	case NOISE_IMAGE_FILE_FORMAT_NOT_SUPPORTED:
	default:
		ERROR_MSG("ITexture: failed to save texture. image file format not supported!!");
		return false;//invalid
		break;
	}



	//Unmap() and release temporary texture2d
	g_pImmediateContext->Unmap(pCpuReadTexture, 0);
	ReleaseCOM(pCpuReadTexture);

	//Save to File
	IFileIO fileIO;
	bool saveSucceeded = fileIO.ExportFile_PURE(filePath, (uint8_t*)outFileData.GetBufferPointer(), outFileData.GetBufferSize(), true);

	return saveSucceeded;
}

/********************************************************************

									P R I V A T E

********************************************************************/

//invoked by texture manager
void NOISE_MACRO_FUNCTION_EXTERN_CALL Texture2D::mFunction_InitTexture(ID3D11ShaderResourceView * pSRV, const N_UID& uid, std::vector<Color4u>&& pixelBuff, bool isSysMemBuffValid)
{
	m_pSRV = pSRV;
	mTextureUid = uid;
	mIsPixelBufferInMemValid = isSysMemBuffValid;
	mPixelBuffer = pixelBuff;

	//get the size of Texture
	ID3D11Texture2D* pTmpRes;
	D3D11_TEXTURE2D_DESC texDesc;
	//get resource from SRV, then get description from texture2D
	m_pSRV->GetResource((ID3D11Resource**)&pTmpRes);
	pTmpRes->GetDesc(&texDesc);
	ReleaseCOM(pTmpRes);
	mWidth = texDesc.Width;
	mHeight = texDesc.Height;
	mMipMapLevels = texDesc.MipLevels;
	mMipMapChainPixelCount = Ut::ComputeMipMapChainPixelCount(texDesc.MipLevels, mWidth, mHeight);
}
