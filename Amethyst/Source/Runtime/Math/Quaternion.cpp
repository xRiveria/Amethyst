#include "Amethyst.h"

//= Based on ==========================================================================//
// http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/index.htm //
// Heading    -> Yaw        -> Y-axis                                                       //
// Attitude    -> Pitch    -> X-axis                                                       //
// Bank        -> Roll        -> Z-axis                                                       //
//=====================================================================================//


namespace Amethyst::Math
{
	const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

    //= FROM ==================================================================================
    void Quaternion::FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
    {
        *this = Matrix(
            xAxis.m_X, yAxis.m_X, zAxis.m_X, 0.0f,
            xAxis.m_Y, yAxis.m_Y, zAxis.m_Y, 0.0f,
            xAxis.m_Z, yAxis.m_Z, zAxis.m_Z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        ).RetrieveRotation();
    }
    //========================================================================================

    std::string Quaternion::ToString() const
    {
        char tempBuffer[200];
        sprintf_s(tempBuffer, "X:%f, Y:%f, Z:%f, W:%f", x, y, z, w);
        return std::string(tempBuffer);
    }
}