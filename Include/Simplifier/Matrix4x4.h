#ifndef __MATRIX4X4_H__
#define __MATRIX4X4_H__

#include <string.h>
#include "Vector3D.h"
#include "Vector4D.h"
#include "Plane.h"

namespace REMTNS {
	class Matrix4x4
	{
	public:
		float m[4][4];

		// Ctors
		Matrix4x4();
		Matrix4x4(const Matrix4x4& other);
		Matrix4x4(const float* pMtx);

		// Access methods
		void Set(const float* pMtx);
		float* operator [] (int index);
		const float* operator [] (int index) const;
		const Matrix4x4& operator = (const Matrix4x4& other);
		bool operator == (const Matrix4x4& other) const;

		inline float& operator()(int r, int c) { return m[r][c]; }
		inline float operator()(int r, int c) const { return m[r][c]; }

		// Unary operators
		Matrix4x4 operator * (const Matrix4x4& other) const;
		const Matrix4x4& operator *= (const Matrix4x4& other);

		// Other methods 
		Matrix4x4& Transpose();
		Matrix4x4& Invert();
		Matrix4x4 MultiplyMatrix(const Matrix4x4& other) const;
		Vector3D MultiplyPointVector(const Vector3D& vec) const; // uses 1 as w
		Vector3D MultiplyDirectionVector(const Vector3D& vec) const; // uses 0 as w
		Vector4D MultiplyVector(const Vector4D& vec) const;
		REMTNS::Plane MultiplyPlane(const REMTNS::Plane& vec) const;

		Matrix4x4 GetTranspose() const;
		Matrix4x4 GetInverse() const;

		// Get Axies
		inline Vector3D GetXAxis() const;
		inline Vector3D GetYAxis() const;
		inline Vector3D GetZAxis() const;
		inline Vector3D GetTAxis() const;

		inline void SetXAxis(const Vector3D& vec);
		inline void SetYAxis(const Vector3D& vec);
		inline void SetZAxis(const Vector3D& vec);
		inline void SetTAxis(const Vector3D& vec);

		// Set methods
		void SetToZero();
		void SetToIdentity();
		void SetToRotationX(float angle);
		void SetToRotationY(float angle);
		void SetToRotationZ(float angle);
		void SetToFreeAxisRotation(const Vector3D& axis, float angle);
		void SetToTranslation(const Vector3D& translation);
		void SetToProjection(float l, float r, float b, float t, float n, float f);

		// Ortho-normalizes the matrix
		void OrthoNormalize();
	};

	inline float* Matrix4x4::operator [] (int index)
	{
		return m[index];
	}

	inline const float* Matrix4x4::operator [] (int index) const
	{
		return m[index];
	}

	inline const Matrix4x4& Matrix4x4::operator = (const Matrix4x4& other)
	{
		this->Set(&other.m[0][0]);
		return *this;
	}

	inline bool Matrix4x4::operator == (const Matrix4x4& other) const
	{
		return (0 == memcmp(m, other.m, sizeof(float) * 16));
	}

	inline Matrix4x4 Matrix4x4::operator * (const Matrix4x4& other) const
	{
		return this->MultiplyMatrix(other);
	}

	inline const Matrix4x4& Matrix4x4::operator *= (const Matrix4x4& other)
	{
		*this = this->MultiplyMatrix(other);
		return *this;
	}

	inline Vector4D operator*(const Matrix4x4& mat, const Vector4D& v)
	{
		return Vector4D(
			mat(0, 0) * v[0] + mat(0, 1) * v[1] + mat(0, 2) * v[2] + mat(0, 3) * v[3],
			mat(1, 0) * v[0] + mat(1, 1) * v[1] + mat(1, 2) * v[2] + mat(1, 3) * v[3],
			mat(2, 0) * v[0] + mat(2, 1) * v[1] + mat(2, 2) * v[2] + mat(2, 3) * v[3],
			mat(3, 0) * v[0] + mat(3, 1) * v[1] + mat(3, 2) * v[2] + mat(3, 3) * v[3]);
	}

	inline Matrix4x4& operator+=(Matrix4x4& lh, const Matrix4x4& rh)
	{
		for (std::size_t i = 0; i < 4; ++i)
		{
			for (std::size_t j = 0; j < 4; ++j)
			{
				lh.m[i][j] += rh.m[i][j];
			}
		}
		return lh;
	}

	inline Matrix4x4& operator-=(Matrix4x4& lh, const Matrix4x4& rh)
	{
		for (std::size_t i = 0; i < 4; ++i)
		{
			for (std::size_t j = 0; j < 4; ++j)
			{
				lh.m[i][j] -= rh.m[i][j];
			}
		}
		return lh;
	}

	inline Matrix4x4 operator+(const Matrix4x4& lh, const Matrix4x4& rh)
	{
		Matrix4x4 mat;

		for (std::size_t i = 0; i < 4; ++i)
		{
			for (std::size_t j = 0; j < 4; ++j)
			{
				mat.m[i][j] = lh.m[i][j] + rh.m[i][j];
			}
		}
		return mat;
	}

	inline Matrix4x4 operator-(const Matrix4x4& lh, const Matrix4x4& rh)
	{
		Matrix4x4 mat;

		for (std::size_t i = 0; i < 4; ++i)
		{
			for (std::size_t j = 0; j < 4; ++j)
			{
				mat.m[i][j] = lh.m[i][j] - rh.m[i][j];
			}
		}
		return mat;
	}

	inline Vector3D Matrix4x4::MultiplyPointVector(const Vector3D& vec) const
	{
		Vector3D tmp;
		tmp.x = vec.x * m[0][0] + vec.y * m[0][1] + vec.z * m[0][2] + m[0][3];
		tmp.y = vec.x * m[1][0] + vec.y * m[1][1] + vec.z * m[1][2] + m[1][3];
		tmp.z = vec.x * m[2][0] + vec.y * m[2][1] + vec.z * m[2][2] + m[2][3];
		return tmp;
	}

	inline Vector4D Matrix4x4::MultiplyVector(const Vector4D& vec) const
	{
		Vector4D tmp;
		tmp.x = vec.x * m[0][0] + vec.y * m[0][1] + vec.z * m[0][2] + vec.w * m[0][3];
		tmp.y = vec.x * m[1][0] + vec.y * m[1][1] + vec.z * m[1][2] + vec.w * m[1][3];
		tmp.z = vec.x * m[2][0] + vec.y * m[2][1] + vec.z * m[2][2] + vec.w * m[2][3];
		tmp.w = vec.x * m[3][0] + vec.y * m[3][1] + vec.z * m[3][2] + vec.w * m[3][3];
		return tmp;
	}

	inline Vector3D Matrix4x4::MultiplyDirectionVector(const Vector3D& vec) const
	{
		Vector3D tmp;
		tmp.x = vec.x * m[0][0] + vec.y * m[0][1] + vec.z * m[0][2];
		tmp.y = vec.x * m[1][0] + vec.y * m[1][1] + vec.z * m[1][2];
		tmp.z = vec.x * m[2][0] + vec.y * m[2][1] + vec.z * m[2][2];
		return tmp;
	}

	inline Plane Matrix4x4::MultiplyPlane(const Plane& vec) const
	{
		Plane tmp;
		tmp.N.x = vec.N.x * m[0][0] + vec.N.y * m[0][1] + vec.N.z * m[0][2] + vec.D * m[0][3];
		tmp.N.y = vec.N.x * m[1][0] + vec.N.y * m[1][1] + vec.N.z * m[1][2] + vec.D * m[1][3];
		tmp.N.z = vec.N.x * m[2][0] + vec.N.y * m[2][1] + vec.N.z * m[2][2] + vec.D * m[2][3];
		tmp.D = vec.N.x * m[3][0] + vec.N.y * m[3][1] + vec.N.z * m[3][2] + vec.D * m[3][3];
		return tmp;
	}

	inline Vector3D Matrix4x4::GetXAxis() const
	{
		return Vector3D(m[0][0], m[1][0], m[2][0]);
	}

	inline Vector3D Matrix4x4::GetYAxis() const
	{
		return Vector3D(m[0][1], m[1][1], m[2][1]);
	}

	inline Vector3D Matrix4x4::GetZAxis() const
	{
		return Vector3D(m[0][2], m[1][2], m[2][2]);
	}

	inline Vector3D Matrix4x4::GetTAxis() const
	{
		return Vector3D(m[0][3], m[1][3], m[2][3]);
	}

	inline void Matrix4x4::SetXAxis(const Vector3D& vec)
	{
		m[0][0] = vec.x; m[1][0] = vec.y; m[2][0] = vec.z;
	}

	inline void Matrix4x4::SetYAxis(const Vector3D& vec)
	{
		m[0][1] = vec.x; m[1][1] = vec.y; m[2][1] = vec.z;
	}

	inline void Matrix4x4::SetZAxis(const Vector3D& vec)
	{
		m[0][2] = vec.x; m[1][2] = vec.y; m[2][2] = vec.z;
	}

	inline void Matrix4x4::SetTAxis(const Vector3D& vec)
	{
		m[0][3] = vec.x; m[1][3] = vec.y; m[2][3] = vec.z;
	}

	inline Matrix4x4 VectorTimesTranspose(const Vector4D& v)
	{
		Matrix4x4 mat;

		mat(0, 0) = v.x * v.x; mat(0, 1) = v.x * v.y; mat(0, 2) = v.x * v.z; mat(0, 3) = v.x * v.w;
		mat(1, 0) = v.y * v.x; mat(1, 1) = v.y * v.y; mat(1, 2) = v.y * v.z; mat(1, 3) = v.y * v.w;
		mat(2, 0) = v.z * v.x; mat(2, 1) = v.z * v.y; mat(2, 2) = v.z * v.z; mat(2, 3) = v.z * v.w;
		mat(3, 0) = v.w * v.x; mat(3, 1) = v.w * v.y; mat(3, 2) = v.w * v.z; mat(3, 3) = v.w * v.w;

		return mat;
	}
}
#endif //__MATRIX4X4_H__