//-----------------------------------------------------------------------------
// File: Vec2d.h
//-----------------------------------------------------------------------------

#pragma once

#include <Windows.h>

template<typename Ty>
class Vector2D {
private:
	constexpr static float EffectiveEffer = 0.0001f;

public:
	Ty x;
	Ty y;
	constexpr Vector2D() noexcept : x(0), y(0) {}
	template<typename OTy>
	constexpr Vector2D(OTy X, OTy Y) noexcept : x(static_cast<Ty>(X)), y(static_cast<Ty>(Y)) {}
	template<typename OTy>
	constexpr Vector2D(const Vector2D<OTy>& other) noexcept : x(static_cast<Ty>(other.x)), y(static_cast<Ty>(other.y)) {}
	template<typename OTy>
	void Set(OTy X, OTy Y) { x = (static_cast<Ty>(X)); y = (static_cast<Ty>(Y)); }


	Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
	Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
	Vector2D& operator*=(const float f) { x *= f; y *= f; return *this; }
	Vector2D& operator*=(const int i) { x *= i; y *= i; return *this; }
	Vector2D& operator=(const POINT& other) { x = other.x; y = other.y; return *this; }
	Vector2D& operator=(const Vector2D& other) { x = other.x; y = other.y; return *this; }
	Vector2D operator-() const { return{-x, -y}; }

	operator POINT() const { return POINT{ static_cast<LONG>(x) ,static_cast<LONG>(y) }; }
	Vector2D Normalize() { float fLength = Length(); if (fLength != 0) { static_cast<float>(x) /= fLength; static_cast<float>(y) /= fLength; } return *this; }
	constexpr float Length() const { return(sqrt(x * x + y * y)); }

	bool operator==(const Vector2D& other) const { return(fabs(x - other.x) < EffectiveEffer && fabs(y - other.y) < EffectiveEffer); }
	bool operator!=(const Vector2D& other) const { return !(operator==(other)); }

};

using Vec2sz = Vector2D<size_t>;
using Vec2i = Vector2D<int>;
using Vec2f = Vector2D<float>;

template<typename Ty>
inline Vector2D<Ty> operator+(const Vector2D<Ty> a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x + b.x, a.y + b.y); }
template<typename Ty>
inline Vector2D<Ty> operator-(const Vector2D<Ty> a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x - b.x, a.y - b.y); }

template<typename Ty>
inline Vector2D<Ty> operator+(const POINT a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x + b.x, a.y + b.y); }
template<typename Ty>
inline Vector2D<Ty> operator+(const Vector2D<Ty> a, const POINT b) { return Vector2D<Ty>(a.x + b.x, a.y + b.y); }
template<typename Ty>
inline Vector2D<Ty> operator-(const POINT a, const Vector2D<Ty> b) { return Vector2D<Ty>(a.x - b.x, a.y - b.y); }
template<typename Ty>
inline Vector2D<Ty> operator-(const Vector2D<Ty> a, const POINT b) { return Vector2D<Ty>(a.x - b.x, a.y - b.y); }

template<typename Ty>
inline float operator*(const Vector2D<Ty> a, const Vector2D<Ty> b) { return (a.x * b.x + a.y * b.y); }
template<typename Ty>
inline Vector2D<Ty> operator*(const float a, const Vector2D<Ty> b) { return Vector2D<Ty>(a * b.x, a * b.y); }
template<typename Ty>
inline Vector2D<Ty> operator*(const Vector2D<Ty> a, const float b) { return Vector2D<Ty>(b * a.x, b * a.y); }

template<typename Vector2F = POINT>
float Length(const Vector2F& pd2dptVector) { return(sqrt(pd2dptVector.x * pd2dptVector.x + pd2dptVector.y * pd2dptVector.y)); }

template<typename Vector2F = POINT>
void Normalize(Vector2F& pd2dptVector) { float fLength = Length(pd2dptVector); if (fLength != 0) { pd2dptVector.x /= fLength; pd2dptVector.y /= fLength; } }

template<typename VecTy1 = POINT, typename VecTy2 = POINT>
inline bool IsPointInCircle(VecTy1 ptCenter, float fRadius, VecTy2 pt)
{
	auto vDelta = ptCenter - pt;
//	float dx = ptCenter.x - pt.x;
//	float dy = ptCenter.y - pt.y;

	return{ vDelta.Length() <= fRadius };
//	float length = sqrt(dx * dx + dy * dy);
//	return (length <= fRadius);
}

template<typename Vec = POINT>
constexpr inline bool IsIntersectCircle(Vec pt1, float fRadius1, Vec pt2, float fRadius2) noexcept
{ return{ Vec{ pt1 - pt2 }.Length() <= (fRadius1 + fRadius2) }; }
