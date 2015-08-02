
/***********************************************************************

                           h：NoiseType

************************************************************************/

#pragma once


typedef		D3DXVECTOR2		NVECTOR2;
typedef		D3DXVECTOR3		NVECTOR3;
typedef		D3DXVECTOR4		NVECTOR4;
typedef		D3DXMATRIX		NMATRIX;

struct N_DefaultVertex
{
	N_DefaultVertex(){ZeroMemory(this,sizeof(*this));};
	
	NVECTOR3 Pos;
	NVECTOR4 Color;
	NVECTOR3 Normal;
	NVECTOR2 TexCoord;
};

struct N_SimpleVertex
{
	N_SimpleVertex() { ZeroMemory(this, sizeof(*this)); };
	NVECTOR3 Pos;
	NVECTOR4 Color;
};

struct N_Material
{
	N_Material(){ZeroMemory(this,sizeof(*this));}

	NVECTOR3	mAmbientColor;	INT32 mSpecularSmoothLevel;
	NVECTOR3	mDiffuseColor;		float		mPad1;
	NVECTOR3	mSpecularColor;	float		mPad2;

};

struct N_DirectionalLight
{
	N_DirectionalLight()  
	{ 
		ZeroMemory(this,sizeof(*this));
		mSpecularIntensity = 1.0f;
		mDirection	= NVECTOR3(1.0f,0,0);
		mDiffuseIntensity=0.5;
	}
	BOOL operator==(N_DirectionalLight& Light)
	{
		//two memory fragments are identical
		if(memcmp(this,&Light,sizeof(Light))==0)
		{return TRUE;}
		return FALSE;
	}

	NVECTOR3 mAmbientColor;	 float		mSpecularIntensity;
	NVECTOR3 mDiffuseColor;	float			mDiffuseIntensity;
	NVECTOR3 mSpecularColor;	 float		mPad2;//用于内存对齐
	NVECTOR3 mDirection;			 float		mPad3;//用于内存对齐

};

struct N_PointLight
{
	N_PointLight() { 
		ZeroMemory(this,sizeof(*this)); 
		mSpecularIntensity = 1.0f;
		mAttenuationFactor = 0.05f;
		mLightingRange	=100.0f;
		mDiffuseIntensity=0.5;
	}

	BOOL operator==(N_PointLight& Light)
	{
		//two memory fragments are identical
		if(memcmp(this,&Light,sizeof(Light))==0)
		{return TRUE;}
		return FALSE;
	}

	NVECTOR3 mAmbientColor;		float mSpecularIntensity;
	NVECTOR3 mDiffuseColor;		float mLightingRange;
	NVECTOR3 mSpecularColor;		float mAttenuationFactor;
	NVECTOR3 mPosition;				float mDiffuseIntensity;//memory alignment

};

struct N_SpotLight
{
	N_SpotLight() 
	{ 
		ZeroMemory(this,sizeof(*this));
		mSpecularIntensity = 1.0f;
		mAttenuationFactor = 1.0f;
		mLightingRange	=100.0f;
		mLightingAngle = MATH_PI / 4;
		mDiffuseIntensity=0.5;
		mLitAt	= NVECTOR3(0,0,0);
	}

	BOOL operator==(N_SpotLight& Light)
	{
		//two memory fragments are identical
		if(memcmp(this,&Light,sizeof(Light))==0)
		{return TRUE;}
		return FALSE;
	}


	NVECTOR3 mAmbientColor;		float mSpecularIntensity;
	NVECTOR3 mDiffuseColor;		float mLightingRange;
	NVECTOR3 mSpecularColor;		float mAttenuationFactor;
	NVECTOR3 mLitAt;					float mLightingAngle;
	NVECTOR3 mPosition;				float mDiffuseIntensity;
};


struct N_CbPerFrame
{
	NMATRIX			mProjMatrix;
	NMATRIX			mViewMatrix;
	N_DirectionalLight mDirectionalLight_Dynamic[10];//放心 已经对齐了
	N_PointLight	 mPointLight_Dynamic[10];
	N_SpotLight	mSpotLight_Dynamic[10];
	int		mDirLightCount_Dynamic;
	int		mPointLightCount_Dynamic;
	int		mSpotLightCount_Dynamic;
	BOOL	mIsLightingEnabled_Dynamic;
	NVECTOR3	mCamPos;	float mPad1;

};

struct N_CbPerObject
{
	NMATRIX	mWorldMatrix;
	NMATRIX	mWorldInvTransposeMatrix;
};

struct N_CbPerSubset
{
	N_Material	mMaterial;
};

struct N_CbRarely
{
	//———————static light————————
	N_DirectionalLight mDirectionalLight_Static[50];
	N_PointLight	 mPointLight_Static[50];
	N_SpotLight	mSpotLight_Static[50];
	int		mDirLightCount_Static;
	int		mPointLightCount_Static;
	int		mSpotLightCount_Static;
	int		mIsLightingEnabled_Static;
};

struct N_CbDrawLine3D
{
	NMATRIX mProjMatrix;
	NMATRIX	mViewMatrix;
};

struct N_LineStrip
{
	N_LineStrip() { ZeroMemory(this, sizeof(*this)); }//pointList = new std::vector<NVECTOR3>; }

	std::vector<NVECTOR3>	pointList;
	UINT		LayerID;
};