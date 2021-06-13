#include "Amethyst.h"

namespace Amethyst::Math
{
	const Matrix Matrix::Identity
	(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	std::string Matrix::ToString() const
	{
		char tempBuffer[200];
		sprintf_s(tempBuffer, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
		return std::string(tempBuffer);
	}
}