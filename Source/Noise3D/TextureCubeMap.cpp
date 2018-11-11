/***********************************************************************

               TextureCubeMap,  interface for cube map

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;



Noise3D::TextureCubeMap::TextureCubeMap()
{
}

Noise3D::TextureCubeMap::~TextureCubeMap()
{
}

NColor4u Noise3D::TextureCubeMap::GetPixel(uint32_t faceID, uint32_t x, uint32_t y)
{
	if (ITexture::IsSysMemBufferValid())
	{
		if (faceID < 6 && x >= 0 && x < mWidth && y >= 0 && y < mHeight)
		{
			UINT pixelIndex =  faceID * mWidth * mHeight +  y*mWidth + x;
			return mPixelBuffer.at(pixelIndex);
		}
		else
		{
			ERROR_MSG("GetPixel : invalid param! out of range!");
		}
	}
	else
	{
		ERROR_MSG("GetPixel : didn't keep a copy in memory !!!");
	}
	return  NColor4u(255, 0, 255, 255);
}

NColor4u Noise3D::TextureCubeMap::GetPixel(NVECTOR3 dir, N_TEXTURE_CPU_SAMPLE_MODE mode)
{
	if (dir.x == 0 && dir.y == 0 && dir.z == 0)
	{
		ERROR_MSG("GetPixel : direction length can't be 0.");
		return NColor4u(255, 0, 255, 255);//error
	}

	if (ITexture::IsSysMemBufferValid())
	{
		//normalize the direction to put its end on a width=2 cube 
		float normalizedFactor = max(max(abs(dir.x), abs(dir.y)), abs(dir.z));
		dir /= -normalizedFactor;
		std::swap(dir.x, dir.z);//right-handed to left-handed

		//cube maps faces: +x, -x, +y, -y, +z, -z
		uint32_t faceID = -1;
		if (Ut::TolerantEqual(dir.x , 1.0f))faceID = 0;
		if (Ut::TolerantEqual(dir.x , -1.0f))faceID =1;
		if (Ut::TolerantEqual(dir.y , 1.0f))faceID = 2;
		if (Ut::TolerantEqual(dir.y , -1.0f))faceID =3;
		if (Ut::TolerantEqual(dir.z , 1.0f))faceID = 4;
		if (Ut::TolerantEqual(dir.z , -1.0f))faceID = 5;

		if (faceID >= 6)
		{
			ERROR_MSG("GetPixel : direction invalid.");
			return NColor4u(0, 255, 255, 255);//error
		}

		uint32_t px = 0;
		uint32_t py = 0;
		float positiveX = (dir.x+ 1.0f)/2.0f;
		float positiveY = (dir.y + 1.0f)/2.0f;
		float positiveZ = (dir.z + 1.0f)/2.0f;
		switch (faceID)
		{
		case 0: px = uint32_t((1.0f-positiveZ) * mWidth); py = uint32_t((1.0f- positiveY) * mHeight); break;//+x
		case 1: px = uint32_t((positiveZ) * mWidth); py = uint32_t((1.0f - positiveY) * mHeight); break;//-x
		case 2: px = uint32_t(positiveX * mWidth); py = uint32_t((positiveZ) * mHeight); break;//+y
		case 3: px = uint32_t((positiveX) * mWidth); py = uint32_t((1.0f - positiveZ) * mHeight); break;//-y
		case 4: px = uint32_t((positiveX) * mWidth); py = uint32_t((1.0f - positiveY) * mHeight); break;//+z
		case 5: px = uint32_t((1.0f-positiveX) * mWidth); py = uint32_t((1.0f-positiveY) * mHeight); break;//-z
		default: break;
		}
		if (px == mWidth)px = mWidth - 1;
		if (py == mHeight)py = mHeight - 1;

		//WARNING: mip maps will affect the index calculation because the memory layout is:
		//+X (level0 1 2 3 .... n) 
		//-X(Level0 1 2....n) 
		//+Y(Level0 1 2....n) 
		//-Y(Level0 1 2....n) 
		//+Z(Level0 1 2....n) 
		//-Z(Level0 1 2....n) 
		//mip map chain pixel pitch is initialized when Texture is firstly loaded.
		uint32_t pixelIndex = faceID * mMipMapChainPixelCount + py *mWidth + px;
		return mPixelBuffer.at(pixelIndex);
	}
	else
	{
		ERROR_MSG("GetPixel : didn't keep a copy in memory !!!");
	}

	return NColor4u(255, 0, 255, 255);
}


/*************************************

					PRIVATE

**************************************/

void NOISE_MACRO_FUNCTION_EXTERN_CALL Noise3D::TextureCubeMap::mFunction_InitTexture(ID3D11ShaderResourceView * pSRV, const N_UID & uid, std::vector<NColor4u>&& pixelBuff, bool isSysMemBuffValid)
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
