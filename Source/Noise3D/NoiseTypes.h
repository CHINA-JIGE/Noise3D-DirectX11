/***********************************************************************

                           h£ºNoiseType

************************************************************************/

#pragma once
using namespace DirectX;

namespace Noise3D
{

	typedef		SimpleMath::Vector2 	NVECTOR2;
	typedef		SimpleMath::Vector3 	NVECTOR3;
	typedef		SimpleMath::Vector4 	NVECTOR4;
	typedef		SimpleMath::Matrix		NMATRIX;
	typedef		SimpleMath::Quaternion NQUATERNION;
	typedef		std::string		NFilePath;
	typedef		std::string		NString;
	typedef		std::string		N_UID;
	typedef		SimpleMath::Color 	NColor4f;

	struct NColor4u
	{
		NColor4u():r(0),g(0),b(0),a(0){}
		NColor4u(const NVECTOR4& c) :r(uint8_t(c.x * 255)), g(uint8_t(c.y * 255)), b(uint8_t(c.z * 255)), a(uint8_t(c.w * 255)) {}
		NColor4u(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) :r(_r),g(_g),b(_b),a(_a) {}

		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

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

	struct N_MinimizedVertex 
	{
		NVECTOR3 pos;
	};

	struct N_AABB
	{
		N_AABB() { max = min = { 0,0,0 }; };
		N_AABB(NVECTOR3 Min, NVECTOR3 Max) { min = Min;max = Max; };
		NVECTOR3 max;
		NVECTOR3 min;
	};

	struct N_LineSegment
	{
		N_LineSegment() {}

		N_LineSegment(const NVECTOR3& _v1, const NVECTOR3& _v2) { vert1 = _v1; vert2 = _v2; }

		NVECTOR3 vert1;
		NVECTOR3 vert2;
	};


};