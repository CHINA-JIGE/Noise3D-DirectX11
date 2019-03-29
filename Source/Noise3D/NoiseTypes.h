/***********************************************************************

				NoiseTypes: some commonly used simple types
				(usually just simple container)

************************************************************************/

#pragma once
using namespace DirectX;

namespace Noise3D
{

	typedef	SimpleMath::Vector2 	NVECTOR2;
	typedef	SimpleMath::Vector3 	NVECTOR3;
	typedef	SimpleMath::Vector4 	NVECTOR4;
	typedef	SimpleMath::Matrix		NMATRIX;
	typedef	SimpleMath::Quaternion NQUATERNION;
	typedef	std::string		NFilePath;
	typedef	std::string		NString;
	typedef	std::string		N_UID;
	typedef	SimpleMath::Color 	NColor4f;

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
		N_DefaultVertex(){ };

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
		N_SimpleVertex() { };
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

	struct N_AABB//Axis-Aligned Bounding Box
	{
		N_AABB(){Reset();};
		N_AABB(NVECTOR3 Min, NVECTOR3 Max) { min = Min;max = Max; };

		//reset to infinite far min & max
		void Reset()
		{
			constexpr float posInf = std::numeric_limits<float>::infinity();
			constexpr float negInf = -posInf;
			min = NVECTOR3(posInf, posInf, posInf);
			max = NVECTOR3(negInf, negInf, negInf);
		}

		bool IsValid()const
		{
			return (max.x > min.x) && (max.y > min.y) && (max.z > min.z);
		}

		//compute the AABB of current AABB and rhs
		void Union(const N_AABB& rhs)
		{
			if (rhs.IsValid())
			{
				if (max.x < rhs.max.x)max.x = rhs.max.x;
				if (max.y < rhs.max.y)max.y = rhs.max.y;
				if (max.z < rhs.max.z)max.z = rhs.max.z;

				if (min.x > rhs.min.x)min.x = rhs.min.x;
				if (min.y > rhs.min.y)min.y = rhs.min.y;
				if (min.z > rhs.min.z)min.z = rhs.min.z;
			}
		}

		//the center/centroid of AABB
		NVECTOR3 Centroid()
		{
			return (max + min) / 2.0f;
		}

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

	struct N_Ray
	{
		N_Ray():origin(NVECTOR3(0,0,0)), dir(NVECTOR3(0, 0, 0)), t_max(std::numeric_limits<float>::infinity()),t_min(0.001f){}
		N_Ray(NVECTOR3 _origin, NVECTOR3 _dir) :origin(_origin), dir(_dir), t_max(std::numeric_limits<float>::infinity()) {}
		N_Ray(NVECTOR3 _origin,NVECTOR3 _dir, float _t_min = 0.001f, float _t_max= std::numeric_limits<float>::infinity()) :origin(_origin),dir(_dir),t_max(_t_max), t_min(_t_min) {}

		//evaluate position given ray's parameter t
		NVECTOR3 Eval(float t) const { return origin + t * dir; }

		NVECTOR3 origin;
		NVECTOR3 dir;//(2019.3.28)could be non-normalized vector
		float t_max;//the maximum acceptable parameter t (e.g. used in ray tracing for maximum distance)
		float t_min;//the epsilon offset of ray to avoid origin hit the surface
	};
};