#ifndef DONYA_MATH_VECTOR_3D_H_
#define DONYA_MATH_VECTOR_3D_H_

#pragma once

#include <Simplifier/MathBase.h>

class Vector3D
{
public:
	typedef float scalar_type;

	float x, y, z;

	// Ctors
	Vector3D();
	Vector3D(float value);
	Vector3D(const Vector3D& other);
	Vector3D(const float* pVec);
	Vector3D(float _x, float _y, float _z);

	// Access methods
	void Set(float _x, float _y, float _z);
	//float & operator [] ( int index ); 
	//const float & operator [] ( int index ) const;

	float& operator[](std::size_t index);
	const float& operator[](std::size_t index) const;

	const Vector3D& operator = (const Vector3D& other);
	bool operator == (const Vector3D& other) const;

	// Unary operators
	Vector3D operator - () const;
	const Vector3D& operator -= (const Vector3D& other);
	const Vector3D& operator += (const Vector3D& other);
	const Vector3D& operator *= (float scalar);
	const Vector3D& operator /= (float scalar);

	// Binary operators
	friend Vector3D operator - (const Vector3D& vec1, const Vector3D& vec2);
	friend Vector3D operator + (const Vector3D& vec1, const Vector3D& vec2);
	friend Vector3D operator * (const Vector3D& vec1, const Vector3D& vec2);
	friend Vector3D operator * (const Vector3D& vec, float scalar);
	friend Vector3D operator * (float scalar, const Vector3D& vec);
	friend Vector3D operator / (const Vector3D& vec, float scalar);

	// Other methods 
	float Dot(const Vector3D& other) const;
	Vector3D Cross(const Vector3D& other) const;
	float Length() const;
	float LengthSquared() const;
	Vector3D& Normalize();

	std::size_t LargestComponent() const;

	friend float Dot(const Vector3D& vec1, const Vector3D& vec2);
	friend Vector3D Cross(const Vector3D& vec1, const Vector3D& vec2);
	friend float Distance(const Vector3D& vec1, const Vector3D& vec2);
	friend float DistanceSquared(const Vector3D& vec1, const Vector3D& vec2);
	friend void Normalize(Vector3D& vec1);

	void SetToTriangleNormal(
		const Vector3D& v1,
		const Vector3D& v2,
		const Vector3D& v3
	);

	inline bool IsNaN() const
	{
		return (x != x) || (y != y) || (z != z);
	}

};

inline Vector3D::Vector3D()
{
	x = y = z = 0.f;
}

inline Vector3D::Vector3D(float value)
{
	x = y = z = value;
}

inline Vector3D::Vector3D(const Vector3D& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

inline Vector3D::Vector3D(const float* pVec)
{
	x = pVec[0];
	y = pVec[1];
	z = pVec[2];
}

inline Vector3D::Vector3D(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

inline void Vector3D::Set(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

inline float& Vector3D::operator[](std::size_t index)
{
	return ((float*)&x)[index];
}

inline const float& Vector3D::operator[](std::size_t index) const
{
	return ((const float*)&x)[index];
}

inline const Vector3D& Vector3D::operator = (const Vector3D& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

inline bool Vector3D::operator == (const Vector3D& other) const
{
	return ((x == other.x) && (y == other.y) && (z == other.z));
}

inline Vector3D Vector3D::operator - () const
{
	return Vector3D(-x, -y, -z);
}

inline const Vector3D& Vector3D::operator -= (const Vector3D& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

inline const Vector3D& Vector3D::operator += (const Vector3D& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

inline const Vector3D& Vector3D::operator *= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

inline const Vector3D& Vector3D::operator /= (float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

inline Vector3D operator - (const Vector3D& vec1, const Vector3D& vec2)
{
	Vector3D tmp(vec1);
	tmp -= vec2;
	return tmp;
}

inline Vector3D operator + (const Vector3D& vec1, const Vector3D& vec2)
{
	Vector3D tmp(vec1);
	tmp += vec2;
	return tmp;
}

inline Vector3D operator * (const Vector3D& vec1, const Vector3D& vec2)
{
	Vector3D tmp(vec1);
	tmp[0] = vec1[0] * vec2[0];
	tmp[1] = vec1[1] * vec2[1];
	tmp[2] = vec1[2] * vec2[2];
	return tmp;
}

inline Vector3D operator * (const Vector3D& vec, float scalar)
{
	Vector3D tmp(vec);
	tmp *= scalar;
	return tmp;
}

inline Vector3D operator * (float scalar, const Vector3D& vec)
{
	Vector3D tmp(vec);
	tmp *= scalar;
	return tmp;
}

inline Vector3D operator / (const Vector3D& vec, float scalar)
{
	Vector3D tmp(vec);
	tmp /= scalar;
	return tmp;
}

inline float Vector3D::Dot(const Vector3D& other) const
{
	return (x * other.x + y * other.y + z * other.z);
}

inline float Dot(const Vector3D& vec1, const Vector3D& vec2)
{
	return vec1.Dot(vec2);
}

inline Vector3D Cross(const Vector3D& vec1, const Vector3D& vec2)
{
	return vec1.Cross(vec2);
}

inline float Vector3D::Length() const
{
	return sqrtf(this->LengthSquared());
}

inline float Vector3D::LengthSquared() const
{
	return Dot(*this);
}

inline std::size_t Vector3D::LargestComponent() const
{
	std::size_t index = 0;
	if (y > x) index = 1;
	if (z > (*this)[index]) index = 2;

	return index;
}

inline Vector3D& Vector3D::Normalize()
{
	float r = this->Length();
	if (r == 0)
		return *this;
	r = 1 / r;
	this->x *= r;
	this->y *= r;
	this->z *= r;

	return *this;
}

inline void Normalize(Vector3D& vec1)
{
	vec1.Normalize();
}

inline float DistanceSquared(const Vector3D& vec1, const Vector3D& vec2)
{
	Vector3D v = vec1 - vec2;
	return v.LengthSquared();
}

inline float Distance(const Vector3D& vec1, const Vector3D& vec2)
{
	return sqrtf(DistanceSquared(vec1, vec2));
}

inline Vector3D Vector3D::Cross(const Vector3D& other) const
{
	return Vector3D(y * other.z - z * other.y,
		z * other.x - x * other.z,
		x * other.y - y * other.x);
}

inline void Vector3D::SetToTriangleNormal(
	const Vector3D& v1,
	const Vector3D& v2,
	const Vector3D& v3
)
{
	Vector3D Q;
	Vector3D P;

	Q = v2 - v1;
	P = v3 - v1;

	*this = ::Cross(Q, P);
	this->Normalize();
}

#endif
