/***********************************************************************

                      h£ºShadow (map) related common interface

************************************************************************/
#pragma once

namespace Noise3D
{
	//usually some certain solid objects like mesh
	class IShadowCaster
	{
	public:

		IShadowCaster();

		void EnableCastingShadow(bool enabledSC);//should this light can cast shadow

		bool IsCastingShadowEnabled();

	protected:

		enum SHADOW_MAP_PROJECTION_TYPE
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		};

		//these 2 init functions are invoked by other class (like lightmanager)
		//dirlight arrSize=1, pointLight arrSize=6(cubemap), spotlight arrSize=1
		bool InitShadowMapPerspective(uint32_t arraySize, uint32_t pixelWidth, uint32_t pixelHeight);

		bool InitShadowMapOrthographic(uint32_t arraySize,float rectRealWidth, float rectRealHeight, uint32_t pixelWidth, uint32_t pixelHeight);

		ID3D11DepthStencilView* GetShadowMapDsv;

	private:

		ID3D11DepthStencilView* m_pShadowMapDSV;

		bool mIsCastingShadowEnabled;

		SHADOW_MAP_PROJECTION_TYPE mShadowMapProjectionType;

		float mRectRealWidth;
		float mRectRealHeight;
		uint32_t mPixelWidth;
		uint32_t mPixelHeight;
	};

	//should this object received shadow (via shadow mapping, done in object's pixel shader)
	class IShadowReceiver
	{
	public:

		IShadowReceiver();
		 
		void EnableReceivingShadow(bool enabledRC);//should this object can receive shadow

		bool IsReceivingShadowEnabled();

		void SetShadowColor(NColor4f c);//including transparency

	protected:

	private:

		bool mIsReceivingShadowEnabled;

		NColor4f mShadowColor;

	};


}