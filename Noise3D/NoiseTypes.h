/***********************************************************************

                           h£ºNoiseType

************************************************************************/

#pragma once

//namespace NGlobal
//{
	typedef		D3DXVECTOR2		NVECTOR2;
	typedef		D3DXVECTOR3		NVECTOR3;
	typedef		D3DXVECTOR4		NVECTOR4;
	typedef		D3DXMATRIX		NMATRIX;
	typedef		std::string				NFilePath;
	typedef		std::string				NString;

	struct N_DefaultVertex
	{
		N_DefaultVertex() { ZeroMemory(this, sizeof(*this)); };

		NVECTOR3 Pos;
		NVECTOR4 Color;
		NVECTOR3 Normal;
		NVECTOR2 TexCoord;
		NVECTOR3 Tangent;
	};

	struct N_SimpleVertex
	{
		N_SimpleVertex() { ZeroMemory(this, sizeof(*this)); };
		N_SimpleVertex(NVECTOR3 inPos, NVECTOR4 inColor, NVECTOR2 inTexCoord) { Pos = inPos;Color = inColor;TexCoord = inTexCoord; };
		bool operator!=(N_SimpleVertex& v) {
			if (v.Color != Color || v.Pos != Pos || v.TexCoord != TexCoord) { return true; }
			else { return false; }
		};
		bool operator==(N_SimpleVertex& v) {
			if (v.Color == Color && v.Pos == Pos && v.TexCoord == TexCoord) { return true; }
			else { return false; }
		};

		NVECTOR3 Pos;
		NVECTOR4 Color;
		NVECTOR2 TexCoord;
	};

	struct N_Material_Basic
	{
		N_Material_Basic()
		{
			ZeroMemory(this, sizeof(*this));
			mBaseAmbientColor = NVECTOR3(0, 0, 0);
			mBaseDiffuseColor = NVECTOR3(0.3f, 1.0f, 0.3f);
			mBaseSpecularColor = NVECTOR3(1.0f, 1.0f, 1.0f);
			mSpecularSmoothLevel = 10;
			mNormalMapBumpIntensity = 0.1f;
			mEnvironmentMapTransparency = 0.3f;
		}

		NVECTOR3	mBaseAmbientColor;	INT32	mSpecularSmoothLevel;
		NVECTOR3	mBaseDiffuseColor;		float		mNormalMapBumpIntensity;
		NVECTOR3	mBaseSpecularColor;	float		mEnvironmentMapTransparency;

	};

	struct N_Material
	{
		N_Material() :
			mMatName(""),
			diffuseMapName(""),
			normalMapName(""),
			specularMapName(""),
			environmentMapName("")
		{ };

		std::string	 mMatName;
		N_Material_Basic baseMaterial;
		std::string diffuseMapName;
		std::string normalMapName;
		std::string specularMapName;
		std::string environmentMapName;
	};

	//correspond to one draw call of MESH
	struct N_MeshSubsetInfo
	{
		N_MeshSubsetInfo() { ZeroMemory(this, sizeof(*this)); }
		UINT		startPrimitiveID;
		UINT		primitiveCount;
		std::string		matName;
	};
//}