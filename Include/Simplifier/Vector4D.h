#ifndef DONYA_MATH_VECTOR_4D_H_
#define DONYA_MATH_VECTOR_4D_H_

#pragma once

#include "MathBase.h"

class Vector4D
{
public:
	typedef float scalar_type;

	float x, y, z, w;
	int index;

	// Ctors
	Vector4D();
	Vector4D(float value);
	Vector4D(const Vector4D& other);
	Vector4D(const float* pVec);
	Vector4D(float _x, float _y, float _z, float _w);

	// Access methods
	void Set(float _x, float _y, float _z, float _w);
	//float & operator [] ( int index ); 
	//const float & operator [] ( int index ) const;

	float& operator[](std::size_t index);
	const float& operator[](std::size_t index) const;

	const Vector4D& operator = (const Vector4D& other);
	bool operator == (const Vector4D& other) const;

	// Unary operators
	Vector4D operator - () const;
	const Vector4D& operator -= (const Vector4D& other);
	const Vector4D& operator += (const Vector4D& other);
	const Vector4D& operator *= (float scalar);
	const Vector4D& operator /= (float scalar);

	// Binary operators
	friend Vector4D operator - (const Vector4D& vec1, const Vector4D& vec2);
	friend Vector4D operator + (const Vector4D& vec1, const Vector4D& vec2);
	friend Vector4D operator * (const Vector4D& vec, float scalar);
	friend Vector4D operator * (float scalar, const Vector4D& vec);
	friend Vector4D operator / (const Vector4D& vec, float scalar);

	// Other methods 
	float Dot(const Vector4D& other) const;
	float Length() const;
	float LengthSquared() const;
	Vector4D& Normalize();

	friend float Dot(const Vector4D& vec1, const Vector4D& vec2);
	friend float Distance(const Vector4D& vec1, const Vector4D& vec2);
	friend float DistanceSquared(const Vector4D& vec1, const Vector4D& vec2);
	friend void Normalize(Vector4D& vec1);

	inline bool IsNaN() const
	{
		return (x != x) || (y != y) || (z != z) || (w != w);
	}
};

inline Vector4D::Vector4D()
{
	x = y = z = w = 0.f;
	index = 0;
}

inline Vector4D::Vector4D(float value)
{
	x = y = z = w = value;
	index = 0;
}

inline Vector4D::Vector4D(const Vector4D& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
	index = other.index;
}

inline Vector4D::Vector4D(const float* pVec)
{
	x = pVec[0];
	y = pVec[1];
	z = pVec[2];
	w = pVec[3];
	index = 0;
}

inline Vector4D::Vector4D(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
	index = 0;
}

inline void Vector4D::Set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
	index = 0;
}

inline float& Vector4D::operator[](std::size_t index)
{
	return ((float*)&x)[index];
}

inline const float& Vector4D::operator[](std::size_t index) const
{
	return ((const float*)&x)[index];
}

inline const Vector4D& Vector4D::operator = (const Vector4D& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
	index = other.index;
	return *this;
}

inline bool Vector4D::operator == (const Vector4D& other) const
{
	return ((x == other.x) && (y == other.y) && (z == other.z) && (w == other.w));
}

inline bool operator!=(const Vector4D& lh, const Vector4D& rh)
{
	return !(lh == rh);
}

inline bool operator < (const Vector4D& lh, const Vector4D& rh)
{
	if (lh.x < rh.x)
		return true;
	if (lh.x > rh.x)
		return false;
	if (lh.y < rh.y)
		return true;
	if (lh.y > rh.y)
		return false;
	if (lh.z < rh.z)
		return true;
	if (lh.z > rh.z)
		return false;
	if (lh.w < rh.w)
		return true;
	return false;
}

inline bool operator > (const Vector4D& lh, const Vector4D& rh)
{
	if (lh < rh)
		return false;
	if (lh == rh)
		return false;
	return true;
}

inline bool operator <= (const Vector4D& lh, const Vector4D& rh)
{
	return !(lh > rh);
}

inline bool operator >= (const Vector4D& lh, const Vector4D& rh)
{
	return !(lh < rh);
}



inline Vector4D Vector4D::operator - () const
{
	return Vector4D(-x, -y, -z, -w);
}

inline const Vector4D& Vector4D::operator -= (const Vector4D& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}

inline const Vector4D& Vector4D::operator += (const Vector4D& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}

inline const Vector4D& Vector4D::operator *= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

inline const Vector4D& Vector4D::operator /= (float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
	return *this;
}

inline Vector4D operator - (const Vector4D& vec1, const Vector4D& vec2)
{
	Vector4D tmp(vec1);
	tmp -= vec2;
	return tmp;
}

inline Vector4D operator + (const Vector4D& vec1, const Vector4D& vec2)
{
	Vector4D tmp(vec1);
	tmp += vec2;
	return tmp;
}

inline Vector4D operator * (const Vector4D& vec, float scalar)
{
	Vector4D tmp(vec);
	tmp *= scalar;
	return tmp;
}

inline Vector4D operator * (float scalar, const Vector4D& vec)
{
	Vector4D tmp(vec);
	tmp *= scalar;
	return tmp;
}

inline Vector4D operator / (const Vector4D& vec, float scalar)
{
	Vector4D tmp(vec);
	tmp /= scalar;
	return tmp;
}

inline float Vector4D::Dot(const Vector4D& other) const
{
	return (x * other.x + y * other.y + z * other.z + w * other.w);
}

inline float Dot(const Vector4D& vec1, const Vector4D& vec2)
{
	return vec1.Dot(vec2);
}

inline float Vector4D::Length() const
{
	return sqrtf(this->LengthSquared());
}

inline float Vector4D::LengthSquared() const
{
	return Dot(*this);
}

inline Vector4D& Vector4D::Normalize()
{
	float r = this->Length();
	r = 1.0f / r;
	this->x *= r;
	this->y *= r;
	this->z *= r;
	this->w *= r;

	return *this;
}

inline void Normalize(Vector4D& vec1)
{
	vec1.Normalize();
}

inline float DistanceSquared(const Vector4D& vec1, const Vector4D& vec2)
{
	Vector4D v = vec1 - vec2;
	return v.LengthSquared();
}

inline float Distance(const Vector4D& vec1, const Vector4D& vec2)
{
	return sqrtf(DistanceSquared(vec1, vec2));
}
#endif
