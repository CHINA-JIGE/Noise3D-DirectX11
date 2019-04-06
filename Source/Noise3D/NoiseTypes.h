/***********************************************************************

				NoiseTypes: some commonly used simple types
				(usually just simple container)

************************************************************************/

#pragma once
using namespace DirectX;

namespace Noise3D
{
	typedef	SimpleMath::Vector2 	Vec2;
	typedef	SimpleMath::Vector3 	Vec3;
	typedef	SimpleMath::Vector4 	Vec4;
	typedef	SimpleMath::Matrix		Matrix;
	typedef	SimpleMath::Quaternion Quaternion;
	typedef	std::string		NFilePath;
	typedef	std::string		NString;
	typedef	std::string		N_UID;
	typedef	SimpleMath::Color 	Color4f;
	typedef	SimpleMath::Vector2 PixelCoord2;

	struct Color4u
	{
		Color4u():r(0),g(0),b(0),a(0){}
		Color4u(const Vec4& c) :r(uint8_t(c.x * 255)), g(uint8_t(c.y * 255)), b(uint8_t(c.z * 255)), a(uint8_t(c.w * 255)) {}
		Color4u(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) :r(_r),g(_g),b(_b),a(_a) {}

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

		Vec3 Pos;
		Vec4 Color;
		Vec3 Normal;
		Vec2 TexCoord;
		Vec3 Tangent;
	};

	struct N_SimpleVertex
	{
		N_SimpleVertex() { };
		N_SimpleVertex(Vec3 inPos, Vec4 inColor, Vec2 inTexCoord) { Pos = inPos;Color = inColor;TexCoord = inTexCoord; };
		
		bool operator!=(N_SimpleVertex& v) {
			if (v.Color != Color || v.Pos != Pos || v.TexCoord != TexCoord) { return true; }
			else { return false; }
		};
		
		bool operator==(N_SimpleVertex& v) {
			if (v.Color == Color && v.Pos == Pos && v.TexCoord == TexCoord) { return true; }
			else { return false; }
		};

		Vec3 Pos;
		Vec4 Color;
		Vec2 TexCoord;
	};

	struct N_MinimizedVertex 
	{
		Vec3 pos;
	};

	struct N_AABB//Axis-Aligned Bounding Box
	{
		N_AABB(){Reset();};
		N_AABB(Vec3 Min, Vec3 Max) { min = Min;max = Max; };

		//reset to infinite far min & max
		void Reset()
		{
			constexpr float posInf = std::numeric_limits<float>::infinity();
			constexpr float negInf = -posInf;
			min = Vec3(posInf, posInf, posInf);
			max = Vec3(negInf, negInf, negInf);
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
		Vec3 Centroid()
		{
			return (max + min) / 2.0f;
		}

		//determine if given point is inside AABB
		bool IsPointInside(Vec3 v)
		{
			return (v.x <= max.x && v.y <= max.y && v.z <= max.z&&
				v.x >= min.x && v.y >= min.y && min.z >= min.z);
		}

		Vec3 max;
		Vec3 min;
	};

	struct N_LineSegment
	{
		N_LineSegment() {}

		N_LineSegment(const Vec3& _v1, const Vec3& _v2) { vert1 = _v1; vert2 = _v2; }

		Vec3 vert1;
		Vec3 vert2;
	};

	struct N_Ray
	{
		N_Ray():origin(Vec3(0,0,0)), dir(Vec3(0, 0, 0)), t_min(0.001f), t_max(std::numeric_limits<float>::infinity()){}
		N_Ray(Vec3 _origin, Vec3 _dir) :origin(_origin), dir(_dir), t_min(0.001f), t_max(std::numeric_limits<float>::infinity()) {}
		N_Ray(Vec3 _origin,Vec3 _dir, float _t_min, float _t_max) :origin(_origin),dir(_dir),t_max(_t_max), t_min(_t_min) {}

		//evaluate position given ray's parameter t
		Vec3 Eval(float t) const { return origin + t * dir; }

		float Distance(float t) const { return (t * dir).Length(); }

		Vec3 origin;
		float t_min;//the epsilon offset of ray to avoid origin hit the surface
		Vec3 dir;//(2019.3.28)could be non-normalized vector
		float t_max;//the maximum acceptable parameter t (e.g. used in ray tracing for maximum distance)
	};
};