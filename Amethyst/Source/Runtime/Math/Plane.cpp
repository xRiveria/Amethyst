#include "Amethyst.h"
#include "Plane.h"

namespace Amethyst::Math
{
	Plane::Plane(const Vector3& normal, float distanceFromOrigin)
	{
		this->m_Normal = normal;
		this->m_DistanceFromOrigin = distanceFromOrigin;
	}

	Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
	{
		const Vector3 distanceAToB = b - a;
		const Vector3 distanceAToC = c - a;

		const Vector3 cross = Vector3::Cross(distanceAToB, distanceAToC);
		this->m_Normal = Vector3::Normalize(cross);
		this->m_DistanceFromOrigin = -Vector3::Dot(m_Normal, a);
	}

    Plane::Plane(const Vector3& normal, const Vector3& point)
    {
        this->m_Normal = normal.Normalized();
        m_DistanceFromOrigin = -this->m_Normal.Dot(point);
    }

    void Plane::Normalize()
    {
        Plane result;

        result.m_Normal = Vector3::Normalize(this->m_Normal);
        const float nominator = sqrtf(result.m_Normal.m_X * result.m_Normal.m_X + result.m_Normal.m_Y * result.m_Normal.m_Y + result.m_Normal.m_Z * result.m_Normal.m_Z);
        const float denominator = sqrtf(this->m_Normal.m_X * this->m_Normal.m_X + this->m_Normal.m_Y * this->m_Normal.m_Y + this->m_Normal.m_Z * this->m_Normal.m_Z);
        const float fentity = nominator / denominator;
        result.m_DistanceFromOrigin = this->m_DistanceFromOrigin * fentity;

        this->m_Normal = result.m_Normal;
        this->m_DistanceFromOrigin = result.m_DistanceFromOrigin;
    }

    Plane Plane::Normalize(const Plane& plane)
    {
        Plane newPlane = plane;
        newPlane.Normalize();
        return newPlane;
    }

    float Plane::Dot(const Vector3& v) const
    {
        return (this->m_Normal.m_X * v.m_X) + (this->m_Normal.m_Y * v.m_Y) + (this->m_Normal.m_Z * v.m_Z) + this->m_DistanceFromOrigin;
    }

    float Plane::Dot(const Plane& p, const Vector3& v)
    {
        const Plane& newPlane = p;
        return newPlane.Dot(v);
    }
}