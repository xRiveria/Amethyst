#include "Amethyst.h"
#include "Frustum.h"

namespace Amethyst::Math
{
    Frustum::Frustum(const Matrix& view, const Matrix& projection, float screen_depth)
    {
        // Calculate the minimum m_Z distance in the frustum.
        const float z_min = -projection.m32 / projection.m22;
        const float r = screen_depth / (screen_depth - z_min);
        Matrix projection_updated = projection;
        projection_updated.m22 = r;
        projection_updated.m32 = -r * z_min;

        // Create the frustum matrix from the view matrix and updated projection matrix.
        const Matrix view_projection = view * projection_updated;

        // Calculate near plane of frustum.
        m_Planes[0].m_Normal.m_X = view_projection.m03 + view_projection.m02;
        m_Planes[0].m_Normal.m_Y = view_projection.m13 + view_projection.m12;
        m_Planes[0].m_Normal.m_Z = view_projection.m23 + view_projection.m22;
        m_Planes[0].m_DistanceFromOrigin = view_projection.m33 + view_projection.m32;
        m_Planes[0].Normalize();

        // Calculate far plane of frustum.
        m_Planes[1].m_Normal.m_X = view_projection.m03 - view_projection.m02;
        m_Planes[1].m_Normal.m_Y = view_projection.m13 - view_projection.m12;
        m_Planes[1].m_Normal.m_Z = view_projection.m23 - view_projection.m22;
        m_Planes[1].m_DistanceFromOrigin = view_projection.m33 - view_projection.m32;
        m_Planes[1].Normalize();

        // Calculate left plane of frustum.
        m_Planes[2].m_Normal.m_X = view_projection.m03 + view_projection.m00;
        m_Planes[2].m_Normal.m_Y = view_projection.m13 + view_projection.m10;
        m_Planes[2].m_Normal.m_Z = view_projection.m23 + view_projection.m20;
        m_Planes[2].m_DistanceFromOrigin = view_projection.m33 + view_projection.m30;
        m_Planes[2].Normalize();

        // Calculate right plane of frustum.
        m_Planes[3].m_Normal.m_X = view_projection.m03 - view_projection.m00;
        m_Planes[3].m_Normal.m_Y = view_projection.m13 - view_projection.m10;
        m_Planes[3].m_Normal.m_Z = view_projection.m23 - view_projection.m20;
        m_Planes[3].m_DistanceFromOrigin = view_projection.m33 - view_projection.m30;
        m_Planes[3].Normalize();

        // Calculate top plane of frustum.
        m_Planes[4].m_Normal.m_X = view_projection.m03 - view_projection.m01;
        m_Planes[4].m_Normal.m_Y = view_projection.m13 - view_projection.m11;
        m_Planes[4].m_Normal.m_Z = view_projection.m23 - view_projection.m21;
        m_Planes[4].m_DistanceFromOrigin = view_projection.m33 - view_projection.m31;
        m_Planes[4].Normalize();

        // Calculate bottom plane of frustum.
        m_Planes[5].m_Normal.m_X = view_projection.m03 + view_projection.m01;
        m_Planes[5].m_Normal.m_Y = view_projection.m13 + view_projection.m11;
        m_Planes[5].m_Normal.m_Z = view_projection.m23 + view_projection.m21;
        m_Planes[5].m_DistanceFromOrigin = view_projection.m33 + view_projection.m31;
        m_Planes[5].Normalize();
    }

    bool Frustum::IsVisible(const Vector3& center, const Vector3& extent, bool ignore_near_plane /*= false*/) const
    {
        float radius = 0.0f;

        if (!ignore_near_plane)
        {
            radius = Utilities::Max3(extent.m_X, extent.m_Y, extent.m_Z);
        }
        else
        {
            constexpr float m_Z = std::numeric_limits<float>::infinity(); // reverse-m_Z only (but I must read form Renderer)
            radius = Utilities::Max3(extent.m_X, extent.m_Y, m_Z);
        }

        // Check sphere first as it's cheaper
        //if (CheckSphere(center, radius) != Outside)
        //   return true;

        if (CheckCube(center, radius) != Outside)
            return true;

        return false;
    }

    Intersection Frustum::CheckCube(const Vector3& center, const Vector3& extent) const
    {
        Intersection result = Inside;
        Plane plane_abs;

        // Check if any one point of the cube is in the view frustum.

        for (const Plane& plane : m_Planes)
        {
            plane_abs.m_Normal = plane.m_Normal.Abs();
            plane_abs.m_DistanceFromOrigin = plane.m_DistanceFromOrigin;

            const float m_DistanceFromOrigin = center.m_X * plane.m_Normal.m_X + center.m_Y * plane.m_Normal.m_Y + center.m_Z * plane.m_Normal.m_Z;
            const float r = extent.m_X * plane_abs.m_Normal.m_X + extent.m_Y * plane_abs.m_Normal.m_Y + extent.m_Z * plane_abs.m_Normal.m_Z;

            const float d_p_r = m_DistanceFromOrigin + r;
            const float d_m_r = m_DistanceFromOrigin - r;

            if (d_p_r < -plane.m_DistanceFromOrigin)
            {
                result = Outside;
                break;
            }

            if (d_m_r < -plane.m_DistanceFromOrigin)
            {
                result = Intersects;
            }
        }

        return result;
    }
}