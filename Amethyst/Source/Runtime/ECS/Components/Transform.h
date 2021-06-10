#pragma once
#include "IComponent.h"
#include <vector>
#include "../Runtime/Math/Vector3.h"
#include "../Runtime/Math/Quaternion.h"
#include "../Runtime/Math/Matrix.h"

namespace Amethyst
{
	class Transform : public IComponent
	{
	public:
		Transform(Context* engineContext, Entity* entity, uint32_t componentID = 0);
		~Transform() = default;

		// === IComponent ===
		void OnInitialize() override;
		/// void Serialize() override; 
		/// void Deserialize() override;

		void UpdateTransform();

		// Position 
		Math::Vector3 RetrievePosition() const { return m_Matrix.RetrieveTranslation(); }
		const Math::Vector3& RetrievePositionLocal() const { return m_PositionLocal; }
		void SetPosition(const Math::Vector3& position);
		void SetPositionLocal(const Math::Vector3& position);

		// Rotation
		Math::Quaternion RetrieveRotation() const { return m_Matrix.RetrieveRotation(); }
		const Math::Quaternion& RetrieveRotationLocal() const { return m_RotationLocal; }
		void SetRotation(const Math::Quaternion& rotation);		 ///
		void SetRotationLocal(const Math::Quaternion& rotation); ///

		// Scale
		Math::Vector3 RetrieveScale() const { return m_Matrix.RetrieveScale(); }
		const Math::Vector3& RetrieveScaleLocal() const { return m_ScaleLocal; }
		void SetScale(const Math::Vector3& scale);
		void SetScaleLocal(const Math::Vector3& scale);

		// Translation/Rotation
		void Translate(const Math::Vector3& delta);
		void Rotate(const Math::Quaternion& delta);    ///

		// Directions
		Math::Vector3 RetrieveUpVector()       const;  ///
		Math::Vector3 RetrieveDownVector()     const;  ///
		Math::Vector3 RetrieveForwardVector()  const;  ///
		Math::Vector3 RetrieveBackwardVector() const;  ///
		Math::Vector3 RetrieveRightVector()	   const;  ///
		Math::Vector3 RetrieveLeftVector()	   const;  ///

		// Hierarchy
		bool IsRoot() const { return !HasParent(); }
		bool HasParent() const { return m_Parent; }
		void SetParent(Transform* newParent);

		bool HasChildren() const { return RetrieveChildrenCount() > 0 ? true : false; }
		uint32_t RetrieveChildrenCount() const { return static_cast<uint32_t>(m_Children.size()); }
		void AddChild(Transform* child);
		void BecomeOrphan();

		Transform* RetrieveParent() const { return m_Parent; }
		Transform* RetrieveRoot() { return HasParent() ? RetrieveParent()->RetrieveRoot() : this; }
		Transform* RetrieveChildByIndex(uint32_t index);
		Transform* RetrieveChildByName(const std::string& name);
		const std::vector<Transform*>& RetrieveChildren() const { return m_Children; }

		void AcquireChildren(); // Searches the entire hierarchy, finds any children and saves them in m_Children. This is a recursive function, the children will find their own children and so on.
		bool IsDescendantOf(const Transform* transform) const; // Check if this transform is a child of the argument transform.
		void RetrieveDescendants(std::vector<Transform*>* descendants); // Retrieve children and store in descendants vector.

		// Misc
		void LookAt(const Math::Vector3& vector) { m_LookAt = vector; }
		const Math::Matrix& RetrieveMatrix() const { return m_Matrix; }
		const Math::Matrix& RetrieveMatrixLocal() const { return m_MatrixLocal; }
		const Math::Matrix& RetrieveMatrixPrevious() const { return m_MatrixPrevious; }
		void SetMatrixLastFrame(const Math::Matrix& matrix) { m_MatrixPrevious = matrix; }

	private:
		Math::Matrix RetrieveParentTransformMatrix() const;
		
	private:
		/*
			Imagine the parent is a box and the child is a pencil within the box. If you move/rotate pencil, you apply a local transformation. If you move the box, 
			the pencil will also move. Thus, by perspective of the box, the pencil does not move, hence its local position stays the same. Global position changes 
			for both the box and pencil.
		*/

		// Local - Transform relative to the parent.
		Math::Vector3 m_PositionLocal;
		Math::Quaternion m_RotationLocal;
		Math::Vector3 m_ScaleLocal;

		// World/Global - Transform relative to the world's origin.
		Math::Matrix m_Matrix; // World space matrix.
		Math::Matrix m_MatrixLocal;
		Math::Vector3 m_LookAt;

		Transform* m_Parent; // The parent of this transform.
		std::vector<Transform*> m_Children; // The children of this transform.

		Math::Matrix m_MatrixPrevious;
	};
}