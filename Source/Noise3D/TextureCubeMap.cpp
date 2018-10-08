/***********************************************************************

               TextureCubeMap,  interface for cube map

************************************************************************/
#include "Noise3D.h"

using namespace Noise3D;
using namespace Noise3D::D3D;

Noise3D::TextureCubeMap::~TextureCubeMap()
{
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

}
