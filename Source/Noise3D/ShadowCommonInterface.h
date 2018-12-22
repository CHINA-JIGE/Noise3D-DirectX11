/***********************************************************************

                      h£ºShadow (map) related common interface

************************************************************************/
#pragma once

namespace Noise3D
{
	enum SHADOW_MAP_PROJECTION_TYPE
	{
		ORTHOGRAPHIC,
		PERSPECTIVE
	};

	struct N_SHADOW_MAPPING_PARAM
	{
		N_SHADOW_MAPPING_PARAM() :
			enableShadowMapping(false),
			orthoProjRectWidth(10.0f),
			orthoProjRectHeight(10.0f),
			pixelWidth(256),
			pixelHeight(256) {}

		bool enableShadowMapping;

		float orthoProjRectWidth;//the real width of orthographic projection's shadow map

		float orthoProjRectHeight;//the real height of orthographic projection's shadow map

		uint32_t pixelWidth;//shadow map resolution x

		uint32_t pixelHeight;//shadow map resolution y

	};

	//usually some certain solid objects like mesh
	class IShadowCaster
	{
	public:

		IShadowCaster();

		void EnableCastingShadow(bool enabledSC);//should this light can cast shadow

		bool IsCastingShadowEnabled();

	protected:

		//Init function should be impl by specific shadow caster like "Dynamic Lights"
		virtual bool mFunction_InitShadowMap(SHADOW_MAP_PROJECTION_TYPE type, N_SHADOW_MAPPING_PARAM smParam)=0;
		
		ID3D11DepthStencilView* GetShadowMapDsv();

	private:

		ID3D11DepthStencilView* m_pShadowMapDSV;

		bool mIsCastingShadowEnabled;

		SHADOW_MAP_PROJECTION_TYPE mShadowMapProjectionType;

		float mRectRealWidth;
		float mRectRealHeight;
		uint32_t mPixelWidth;
		uint32_t mPixelHeight;
	};

	//should this object receive shadow (via shadow mapping, shaded in object's pixel shader)
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