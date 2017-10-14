/***********************************************************************

                           h£ºNoiseType

************************************************************************/

#pragma once

namespace Noise3D
{
	typedef		D3DXVECTOR2		NVECTOR2;
	typedef		D3DXVECTOR3		NVECTOR3;
	typedef		D3DXVECTOR4		NVECTOR4;
	typedef		D3DXMATRIX		NMATRIX;
	typedef		std::string				NFilePath;
	typedef		std::string				NString;
	typedef		std::string				N_UID;

	struct N_DefaultVertex
	{
		N_DefaultVertex() { ZeroMemory(this, sizeof(*this)); };

		N_DefaultVertex& operator+=(const N_DefaultVertex& rhs)
		{
			Pos += rhs.Pos;
			Color += rhs.Color;
			Normal += rhs.Normal;
			TexCoord += rhs.TexCoord;
			Tangent += rhs.Tangent;
			return *this;
		};

		N_DefaultVertex operator*=(float factor)
		{
			Pos *= factor;
			Color *= factor;
			Normal *= factor;
			TexCoord *= factor;
			Tangent *= factor;
			return *this;
		};

		bool operator!=(N_DefaultVertex& v) {
			if (v.Pos != Pos || v.Color != Color || v.Normal != Normal || v.TexCoord!=TexCoord || v.Tangent!=Tangent) { return true; }
			else { return false; }
		};

		bool operator==(N_DefaultVertex& v) {
			if (v.Pos == Pos &&  v.Color== Color && v.Normal == Normal && v.TexCoord == TexCoord && v.Tangent == Tangent) { return true; }
			else { return false; }
		};

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

	struct N_MinizedVertex 
	{
		NVECTOR3 pos;
	};

	struct N_Box
	{
		N_Box() { max = min = { 0,0,0 }; };
		N_Box(NVECTOR3 Min, NVECTOR3 Max) { min = Min;max = Max; };
		NVECTOR3 max;
		NVECTOR3 min;
	};

};