
/***********************************************************************

                           cpp£ºITexture, provide interface for user to 
						   modified something

************************************************************************/

#include "Noise3D.h"

using namespace Noise3D;

ITexture::ITexture()
{
	m_pTextureUid = new N_UID("");
	m_pSRV = nullptr;//shader resource view
	mWidth = mHeight = 0;
}

ITexture::~ITexture()
{
	//safe_release SRV  interface
	ReleaseCOM(m_pSRV);
	mPixelBuffer.clear();
}

BOOL ITexture::IsSysMemPixelBufferValid()
{
	return mIsPixelBufferInMemValid;
}

N_UID ITexture::GetTextureName()
{
	return *m_pTextureUid;
}

NOISE_TEXTURE_TYPE ITexture::GetTextureType()
{
	return mTextureType;
}

BOOL ITexture::IsTextureType(NOISE_TEXTURE_TYPE type)
{
	return (type==mTextureType);
}

UINT ITexture::GetWidth()
{
	return mWidth;
}

UINT ITexture::GetHeight()
{
	return mHeight;
}

void ITexture::SetPixel(UINT x, UINT y, const NVECTOR4 & color)
{
	if (mIsPixelBufferInMemValid)
	{
		if (IsTextureType(NOISE_TEXTURE_TYPE_COMMON))
		{
			if (x >= 0 && x < mWidth && y >= 0 && y < mHeight)
			{
				UINT pixelIndex = y*mWidth + x;
				mPixelBuffer.at(pixelIndex) = color;
			}
			else
			{
				ERROR_MSG("SetPixel : Point out of range!");
			}
		}
		else
		{
			ERROR_MSG("Set Pixel : texture type error,Only Common Texture can be modified.");
		}
	}
	else
	{
		ERROR_MSG("SetPixel : Texture ID or Texture Type invalid !!");
	}
}

NVECTOR4 ITexture::GetPixel(UINT x, UINT y)
{
	if (mIsPixelBufferInMemValid)
	{
		if (IsTextureType(NOISE_TEXTURE_TYPE_COMMON))
		{
			if (x >= 0 && x < mWidth && y >= 0 && y < mHeight)
			{
				UINT pixelIndex = y*mWidth + x;
				return mPixelBuffer.at(pixelIndex);
			}
			else
			{
				ERROR_MSG("GetPixel : Point out of range!");
			}
		}
		else
		{
			ERROR_MSG("GetPixel : texture type error,Only Common Texture can be modified.");
		}
	}
	else
	{
		ERROR_MSG("GetPixel : Texture ID or Texture Type invalid !!");
	}
}

BOOL ITexture::UpdateToVideoMemory()
{

	if (IsTextureType(NOISE_TEXTURE_TYPE_COMMON))
	{
		if (IsSysMemPixelBufferValid())
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
			//mIsPixelBufferInMemValid==FALSE
			ERROR_MSG("UpdateTextureToGraphicMemory : Texture didn't have a copy in System Memory!");
			return FALSE;
		}
	}
	else
	{
		//texID = ==Noise_macro_invalid_Texture_ID
		ERROR_MSG("UpdateTextureToGraphicMemory : Texture ID invalid!!");
		return FALSE;
	}

	return TRUE;
}

BOOL ITexture::ConvertTextureToGreyMap()
{
	return ConvertTextureToGreyMapEx(0.3f, 0.59f, 0.1f);
}

BOOL ITexture::ConvertTextureToGreyMapEx(float factorR, float factorG, float factorB)
{
	if (IsTextureType(NOISE_TEXTURE_TYPE_COMMON)==FALSE)
	{
		ERROR_MSG("ConvertTextureToGreyMap:texture Type Invalid!");
		return FALSE;
	}

	//only the texture created both in gpu & memory can be modified 
	//( actually the sysMem block will be modified)
	if (!IsSysMemPixelBufferValid())
	{
		ERROR_MSG("ConvertTextureToGreyMap:Only Textures that keep a copy in memory can be converted ! ");
		return FALSE;
	}

	//Get Dimension info
	D3D11_TEXTURE2D_DESC textureDesc;
	ID3D11Texture2D* pTex2D;
	m_pSRV->GetResource((ID3D11Resource**)&pTex2D);
	pTex2D->GetDesc(&textureDesc);

	UINT picWidth = textureDesc.Width;
	UINT picHeight = textureDesc.Height;

	ReleaseCOM(pTex2D);

	//for every pixel of the copy in memory ,
	//convert their RGBs to same value under some rules
	for (auto& c : mPixelBuffer)
	{
		float greyScale = factorR *c.x + factorG*c.y + factorB*c.z;
		c = NVECTOR4(greyScale, greyScale, greyScale, c.w);
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

	return TRUE;
}

BOOL ITexture::ConvertHeightMapToNormalMap(float heightFieldScaleFactor)
{
	if (IsTextureType(NOISE_TEXTURE_TYPE_COMMON) == FALSE)
	{
		ERROR_MSG("ConvertTextureToNormalMap:texture Type Invalid!");
		return FALSE;
	}

	//only the texture created both in gpu & memory can be modified 
	//( actually the copy in mem will be modified)
	if (!IsSysMemPixelBufferValid())
	{
		ERROR_MSG("ConvertTextureToNormalMap:Only Textures that keep a copy in memory can be converted ! ");
		return FALSE;
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

	//loop to generate normal map
	for (UINT j = 0;j < picHeight - 1;j++)
	{
		for (UINT i = 0;i < picWidth - 1;i++)
		{
			NVECTOR3	currentNormal(0, 0, 0);

			UINT vertexID1=0, vertexID2=0, vertexID3=0;

			//the LSB to indicate if (j==height-1), the 2nd LSB indicate if i==(width-1)
			UINT caseID = ((i == (picWidth-1)) << 1) | (j ==( picHeight-1));
			switch (caseID)
			{
			case 0://common situation
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(i + 1, j);//i+1,j
				vertexID3 = ComputeIndex(i, j + 1);//i, j+1
				}

			case 1://the most bottom row(j==height-1)
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(i+1,j);//i+1,j
				vertexID3 = ComputeIndex(i, 0);//i, j+1
				}

			case 2://the most right column (i==width-1)
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(0, j);//i+1,j
				vertexID3 = ComputeIndex(i, j + 1);//i, j+1
				}

			case 3://right bottom corner pixel
				{
				vertexID1 = ComputeIndex(i, j);
				vertexID2 = ComputeIndex(0, j);//i+1,j
				vertexID3 = ComputeIndex(i, 0);//i, j+1
				}

			}

			//after confirm 3 vertices composing a triangle, apply CROSS operation
			NVECTOR4 color1 = mPixelBuffer.at(vertexID1);
			NVECTOR4 color2 = mPixelBuffer.at(vertexID2);
			NVECTOR4 color3 = mPixelBuffer.at(vertexID3);
			//because it's grey map , so we can only use one color channel
			NVECTOR3	v1 = NVECTOR3(1.0f, 0, heightFieldScaleFactor* (color2.x - color1.x));
			NVECTOR3	v2 = NVECTOR3(0, 1.0f, heightFieldScaleFactor* (color3.x - color1.x));
			D3DXVec3Cross(&currentNormal, &v1, &v2);

			//convert normal to Normal Map Color
			mPixelBuffer.at(vertexID1) =
				NVECTOR4((currentNormal.x + 1) / 2, (currentNormal.y + 1) / 2, (currentNormal.z + 1) / 2, 1.0f);
		}
	}


	//after modifying buffer in memory, update to GPU
	ID3D11Resource* pTmpRes;
	//resource reference count will increase ,so remember to release
	//so getResource() actually gets a interface to the resource BOUND to the view.
	pTexObj->m_pSRV->GetResource(&pTmpRes);

	//update resource which is bound to the corresponding SRV
	g_pImmediateContext->UpdateSubresource(
		pTmpRes,
		0,
		NULL,
		&pTexObj->mPixelBuffer.at(0),
		picWidth * NOISE_MACRO_DEFAULT_COLOR_BYTESIZE,
		NULL);

	ReleaseCOM(pTmpRes);

	return TRUE;
}

BOOL ITexture::SaveTextureToFile(NFilePath filePath, NOISE_TEXTURE_SAVE_FORMAT picFormat)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D* tmp_pResource;
	m_pSRV->GetResource((ID3D11Resource**)&tmp_pResource);
	//use d3dx11
	hr = D3DX11SaveTextureToFileA(
		g_pImmediateContext,
		tmp_pResource,
		D3DX11_IMAGE_FILE_FORMAT(picFormat),
		filePath.c_str()
	);
	HR_DEBUG(hr, "ITexture £º Save Texture Failed!");
	ReleaseCOM(tmp_pResource);

	return TRUE;
}


/********************************************************************

									P R I V A T E

********************************************************************/

//invoked by texture manager
void NOISE_MACRO_FUNCTION_EXTERN_CALL ITexture::mFunction_InitTexture(ID3D11ShaderResourceView * pSRV, const N_UID& uid, std::vector<NVECTOR4>&& pixelBuff, BOOL isSysMemBuffValid, NOISE_TEXTURE_TYPE type)
{
	m_pSRV = pSRV;
	*m_pTextureUid = uid;
	mIsPixelBufferInMemValid = isSysMemBuffValid;
	mPixelBuffer = pixelBuff;
	mTextureType = type;

	//get the size of Texture
	ID3D11Texture2D* pTmpRes;
	D3D11_TEXTURE2D_DESC texDesc;
	//get resource from SRV, then get description from texture2D
	m_pSRV->GetResource((ID3D11Resource**)&pTmpRes);
	pTmpRes->GetDesc(&texDesc);
	ReleaseCOM(pTmpRes);
	mWidth = texDesc.Width;
	mHeight = texDesc.Height;

}
