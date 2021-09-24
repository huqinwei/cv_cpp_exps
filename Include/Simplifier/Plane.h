#ifndef __PLANE_H__
#define __PLANE_H__

#include "Vector3D.h"
#include "Vector4D.h"
namespace REMTNS {
	class Plane
	{
	public:
		Vector3D N;
		float D;

		Plane();
		Plane(const Plane& other);
		Plane(const float* pPlane);
		Plane(const Vector3D& n, float d);

		void NormalizeNormal();

		// Query methods
		float HeightOverPlane(const Vector3D& point) const;
		bool IsPointAbovePlane(const Vector3D& point) const;
		bool IsPointOnPlane(const Vector3D& point) const;
		bool IsPointBelowPlane(const Vector3D& point) const;
		bool IsPointOnOrBelowPlane(const Vector3D& point) const;
		bool IsPointOnOrAbovePlane(const Vector3D& point) const;

		int GetSide(const Vector3D& pt) const;
	};

	inline Plane::Plane() : D(0.f)
	{
	}

	inline Plane::Plane(const Plane& other) : N(other.N), D(other.D)
	{
	}

	inline Plane::Plane(const float* pPlane) : N(pPlane), D(pPlane[3])
	{
	}

	inline Plane::Plane(const Vector3D& n, float d)
		: N(n), D(d)
	{
	}

	inline float Plane::HeightOverPlane(const Vector3D& point) const
	{
		return (Dot(point, N) + D);
	}

	inline bool Plane::IsPointAbovePlane(const Vector3D& point) const
	{
		return (HeightOverPlane(point) > 0);
	}

	inline bool Plane::IsPointOnPlane(const Vector3D& point) const
	{
		return (HeightOverPlane(point) == 0);
	}

	inline bool Plane::IsPointBelowPlane(const Vector3D& point) const
	{
		return (HeightOverPlane(point) < 0);
	}

	inline bool Plane::IsPointOnOrAbovePlane(const Vector3D& point) const
	{
		return (HeightOverPlane(point) >= 0);
	}

	inline bool Plane::IsPointOnOrBelowPlane(const Vector3D& point) const
	{
		return (HeightOverPlane(point) <= 0);
	}

	inline int Plane::GetSide(const Vector3D& pt) const
	{
		const float epsilon = 0.000001f;
		const float h = HeightOverPlane(pt);

		if (h > epsilon) return  1;
		if (h < -epsilon) return -1;
		return 0;
	}

	inline Vector4D PlaneAsVector(const Plane& p)
	{
		return Vector4D(p.N.x, p.N.y, p.N.z, p.D);
	}

	inline Plane PlaneFromPoints(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2)
	{
		Plane plane;

		const Vector3D ea = (p1 - p0).Normalize();
		const Vector3D eb = (p2 - p0).Normalize();

		plane.N = ea.Cross(eb).Normalize();
		plane.D = -plane.N.Dot(p0);

		return plane;
	}
}
#endif //__PLANE_H__