#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "MathUtilities.h"

namespace Amethyst::Math
{
	using namespace Amethyst::Math;
	/*
		Column Majoring Ordering. Remember that while it is safe to assign matrix values regardless of memory ordering, always return new matrix constructions in 
		column-major ordering (swap the places of our value variables accordingly).
	*/
	
	class Matrix
	{
	public:
		Matrix()
		{
			SetIdentity();
		}

		Matrix(const Matrix& matrix)
		{
			m00 = matrix.m00; m01 = matrix.m01; m02 = matrix.m02; m03 = matrix.m03;
			m10 = matrix.m10; m11 = matrix.m11; m12 = matrix.m12; m13 = matrix.m13;
			m20 = matrix.m20; m21 = matrix.m21; m22 = matrix.m22; m23 = matrix.m23;
			m30 = matrix.m30; m31 = matrix.m31; m32 = matrix.m32; m33 = matrix.m33;
		}

		Matrix(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			this->m00 = m00; this->m01 = m01; this->m02 = m02; this->m03 = m03;
			this->m10 = m10; this->m11 = m11; this->m12 = m12; this->m13 = m13;
			this->m20 = m20; this->m21 = m21; this->m22 = m22; this->m23 = m23;
			this->m30 = m30; this->m31 = m31; this->m32 = m32; this->m33 = m33;
		}

		Matrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
		{
			const Matrix m_Rotation = CreateRotation(rotation);

			m00 = scale.m_X * m_Rotation.m00; m01 = scale.m_X * m_Rotation.m01; m02 = scale.m_X * m_Rotation.m02; m03 = 0.0f;
			m10 = scale.m_Y * m_Rotation.m10; m11 = scale.m_Y * m_Rotation.m11; m12 = scale.m_Y * m_Rotation.m12; m13 = 0.0f;
			m20 = scale.m_Z * m_Rotation.m20; m21 = scale.m_Z * m_Rotation.m21; m22 = scale.m_Z * m_Rotation.m22; m23 = 0.0f;
			m30 = translation.m_X;			  m31 = translation.m_Y;		    m32 = translation.m_Z;			  m33 = 1.0f;
		}
		
		~Matrix() = default;

		//Translation
		[[nodiscard]] Vector3 RetrieveTranslation() const { return Vector3(m30, m31, m32); }

		static inline Matrix CreateTranslation(const Vector3& translation)
		{
			return Matrix(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				translation.m_X, translation.m_Y, translation.m_Z, 1
			);
		}

		//Rotation
		static inline Matrix CreateRotation(const Quaternion& rotation)
		{
			const float number9 = rotation.m_X * rotation.m_X;
			const float number8 = rotation.m_Y * rotation.m_Y;
			const float number7 = rotation.m_Z * rotation.m_Z;
			const float number6 = rotation.m_X * rotation.m_Y;
			const float number5 = rotation.m_Z * rotation.m_W;
			const float number4 = rotation.m_Z * rotation.m_X;
			const float number3 = rotation.m_Y * rotation.m_W;
			const float number2 = rotation.m_Y * rotation.m_Z;
			const float number  = rotation.m_X * rotation.m_W;

			

		
		}

		[[nodiscard]] Quaternion RetrieveRotation() const
		{
			const Vector3 scale = RetrieveScale();

			//Avoid division by 0 (we will divide to remove scaling).
			if (scale.m_X == 0.0f || scale.m_Y == 0.0f || scale.m_Z == 0.0f) { return Quaternion(0, 0, 0, 1); }

			//Extract rotation and remove scaling. 
			Matrix normalizedMatrix;

			normalizedMatrix.m00 = m00 / scale.m_X; normalizedMatrix.m01 = m01 / scale.m_X; normalizedMatrix.m02 = m02 / scale.m_X; normalizedMatrix.m03 = 0.0f;
			normalizedMatrix.m10 = m10 / scale.m_Y; normalizedMatrix.m11 = m11 / scale.m_Y; normalizedMatrix.m12 = m12 / scale.m_Y; normalizedMatrix.m13 = 0.0f;
			normalizedMatrix.m20 = m20 / scale.m_Z; normalizedMatrix.m21 = m21 / scale.m_Z; normalizedMatrix.m22 = m22 / scale.m_Z; normalizedMatrix.m23 = 0.0f;
			normalizedMatrix.m30 = 0.0f;            normalizedMatrix.m31 = 0.0f;			normalizedMatrix.m32 = 0.0f;			normalizedMatrix.m33 = 1.0f;
		
			return RotationMatrixToQuaternion(normalizedMatrix);
		}

		static inline Quaternion RotationMatrixToQuaternion(const Matrix& rotation)
		{

		}

		//Scale
		[[nodiscard]] Vector3 RetrieveScale() const
		{
			//Check if our values are signed. Remember that our scale is made up 3 main values.
			const int xSigned = (Utilities::Sign(m00 * m01 * m02 * m03) < 0) ? -1 : 1;
			const int ySigned = (Utilities::Sign(m10 * m11 * m12 * m13) < 0) ? -1 : 1;
			const int zSigned = (Utilities::Sign(m20 * m21 * m22 * m23) < 0) ? -1 : 1;

			return Vector3(
				static_cast<float>(xSigned) * Utilities::SquareRoot(m00 * m00 + m01 * m01 + m02 * m02),
				static_cast<float>(ySigned) * Utilities::SquareRoot(m10 * m10 + m11 * m11 + m12 * m12),
				static_cast<float>(zSigned) * Utilities::SquareRoot(m20 * m20 + m21 * m21 * m22 * m22)
			);
		}

		static inline Matrix CreateScale(float scale) { return CreateScale(scale, scale, scale); }

		static inline Matrix CreateScale(const Vector3& scale) { return CreateScale(scale.m_X, scale.m_Y, scale.m_Z); }

		static inline Matrix CreateScale(float scaleX, float scaleY, float scaleZ)
		{
			return Matrix
			{
				scaleX, 0, 0, 0,
				0, scaleY, 0, 0,
				0, 0, scaleZ, 0,
				0, 0, 0, 1
			};
		}

		//Transpose into Column Major ordering.
		[[nodiscard]] Matrix Transposed() const { return Transpose(*this); }
		void Transpose() { *this = Transpose(*this); }
		static inline Matrix Transpose(const Matrix& matrix)
		{
			return Matrix(
				matrix.m00, matrix.m10, matrix.m20, matrix.m30,
				matrix.m01, matrix.m11, matrix.m21, matrix.m31,
				matrix.m02, matrix.m12, matrix.m22, matrix.m32,
				matrix.m03, matrix.m13, matrix.m23, matrix.m33
			);
		}

		void Decompose(Vector3& scale, Quaternion& rotation, Vector3& translation) const
		{
			translation = RetrieveTranslation();
			scale = RetrieveScale();
			rotation = RetrieveRotation();
		}

		void SetIdentity()
		{
			m00 = 1.0f, m01 = 0.0f, m02 = 0.0f, m03 = 0.0f;
			m10 = 0.0f, m11 = 1.0f, m12 = 0.0f, m13 = 0.0f;
			m20 = 0.0f, m21 = 0.0f, m22 = 1.0f, m23 = 0.0f;
			m30 = 0.0f, m31 = 0.0f, m32 = 0.0f, m33 = 1.0f;
		}

		//Multiplication
		Vector3 operator*(const Vector3& vector) const
		{
			Vector4 workingVector;

			workingVector.m_X = (m00 * vector.m_X) + (m10 * vector.m_X) + (m20 * vector.m_Z) + m30;
			workingVector.m_Y = (m01 * vector.m_X) + (m11 * vector.m_Y) + (m21 * vector.m_Z) + m31;
			workingVector.m_Z = (m02 * vector.m_Z) + (m12 * vector.m_Z) + (m22 * vector.m_Z) + m32;
			workingVector.m_W = 1 / ((m03 * vector.m_X) + (m13 * vector.m_Z) + (m23 * vector.m_Z) + m33); ///

			return Vector3(workingVector.m_X * workingVector.m_W, workingVector.m_Y * workingVector.m_W, workingVector.m_Z * workingVector.m_W);
		}

		Vector4 operator*(const Vector4& vector) const
		{
			return Vector4
			{
				((m00 * vector.m_X) + (m10 * vector.m_Y) + (m20 * vector.m_Z) + (m30 * vector.m_W)),
				((m01 * vector.m_X) + (m11 * vector.m_Y) + (m21 * vector.m_Z) + (m31 * vector.m_W)),
				((m02 * vector.m_X) + (m12 * vector.m_Y) + (m22 * vector.m_Z) + (m32 * vector.m_W)),
				((m03 * vector.m_X) + (m13 * vector.m_Y) + (m23 * vector.m_Z) + (m33 * vector.m_W))
			};
		}
		
		//Comparison
		bool operator==(const Matrix& matrix) const
		{
			const float* ourData = Data();
			const float* otherData = matrix.Data();

			//Compares each of our 16 values with the other matrix's 16 values. We will use episilon here to allow for a small leeway between values.
			for (unsigned int i = 0; i < 16; ++i)
			{
				if (!Utilities::Equals(ourData[i], otherData[i]))
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]] const float* Data() const { return &m00; }
		[[nodiscard]] std::string ToString() const;

	public:
		/*
			Our respective memory representations just determines if our data is stored in row-major or column-major format. As data is stored linearly in memory, we need 
			to have a way to store our matrices (in all its rows and columns glory) in such a linear memory layout. Thus, we can choose to either store them 
			row by row (row-major) or columnm by column (column major). Certain libraries expect data to be in certain memory layouts, so do take note of this.
		*/

		//Column-Major Memory Representation. HLSL expects column-major by default.
		float m00 = 0.0f, m10 = 0.0f, m20 = 0.0f, m30 = 0.0f; //Column 1. Column 0 Row 0, Column 1 Row 0, Column 2 Row 0, Column 3 Row 0.
		float m01 = 0.0f, m11 = 0.0f, m21 = 0.0f, m31 = 0.0f; //Column 2
		float m02 = 0.0f, m12 = 0.0f, m22 = 0.0f, m32 = 0.0f; //Column 3
		float m03 = 0.0f, m13 = 0.0f, m23 = 0.0f, m33 = 0.0f; //Column 4

		/*
		//Row-Major Memory Representation.
		float m00 = 0.0f, m01 = 0.0f, m02 = 0.0f, m03 = 0.0f; //Row 1. Column 0 Row 0, Column 0 Row 1, Column 0 Row 2, Column 0 Row 3.
		float m10 = 0.0f, m11 = 0.0f, m12 = 0.0f, m13 = 0.0f; //Row 2
		float m20 = 0.0f, m21 = 0.0f, m22 = 0.0f, m23 = 0.0f; //Row 3
		float m30 = 0.0f, m31 = 0.0f, m32 = 0.0f, m33 = 0.0f; //Row 4
		*/
	};
}