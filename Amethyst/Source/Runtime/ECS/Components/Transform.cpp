#include "Amethyst.h"
#include "Transform.h"
#include "../World.h"
#include "../Entity.h"


namespace Amethyst
{
	using namespace Math;

	Transform::Transform(Context* engineContext, Entity* entity, uint32_t componentID /*= 0*/) : IComponent(engineContext, entity, componentID, this)
	{
		// Initialize Transform
		m_PositionLocal = Vector3::Zero;
		m_RotationLocal = Quaternion(0, 0, 0, 1);
		m_ScaleLocal = Vector3::One;
		m_Matrix = Matrix::Identity;
		m_MatrixLocal = Matrix::Identity;
		m_MatrixPrevious = Matrix::Identity;
		m_Parent = nullptr;

		REGISTER_ATTRIBUTE_VALUE_VALUE(m_PositionLocal, Vector3);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_RotationLocal, Quaternion);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_ScaleLocal, Vector3);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_Matrix, Matrix);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_MatrixLocal, Matrix);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_LookAt, Vector3);
	}

	void Transform::OnInitialize()
	{
		UpdateTransform();
	}

	void Transform::UpdateTransform()
	{
		// Compute local transform.
		m_MatrixLocal = Matrix(m_PositionLocal, m_RotationLocal, m_ScaleLocal);

		// Compute world transform.
		if (!HasParent)
		{
			m_Matrix = m_MatrixLocal;
		}
		else
		{
			m_Matrix = m_MatrixLocal * RetrieveParentTransformMatrix();
		}

		// Update Children
		for (const auto child : m_Children)
		{
			child->UpdateTransform();
		}
	}

	void Transform::SetPosition(const Vector3& position)
	{
		if (RetrievePosition() == position)
		{
			return;
		}

		SetPositionLocal(!HasParent() ? position : position * RetrieveParent()->RetrieveMatrix().Inverted());
	}

	void Transform::SetPositionLocal(const Vector3& position)
	{
		if (m_PositionLocal == position)
		{
			return;
		}

		m_PositionLocal = position;
		UpdateTransform();
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		/*
			if (RetrieveRotation() == rotation)
			{
				return;
			}

			SetRotationLocal(!HasParent() ? rotation : rotation * RetrieveParent()->RetrieveRotation().Inverse());
		*/
	} 

	void Transform::SetRotationLocal(const Quaternion& rotation)
	{
		/*
			if (m_RotationLocal == rotation)
			{
				return;
			}

			m_RotationLocal = rotation;
			UpdateTransform();
		*/
	}

	void Transform::SetScale(const Vector3& scale)
	{
		if (RetrieveScale() == scale)
		{
			return;
		}

		SetScaleLocal(!HasParent() ? scale : scale / RetrieveParent()->RetrieveScale());
	}

	void Transform::SetScaleLocal(const Vector3& scale)
	{
		if (m_ScaleLocal == scale)
		{
			return;
		}

		m_ScaleLocal = scale;

		// A scale of 0 will cause a division by zero when decomposing the world transform matrix.
		m_ScaleLocal.m_X = (m_ScaleLocal.m_X == 0.0f) ? Math::Utilities::Episilon : m_ScaleLocal.m_X;
		m_ScaleLocal.m_Y = (m_ScaleLocal.m_Y == 0.0f) ? Math::Utilities::Episilon : m_ScaleLocal.m_Y;
		m_ScaleLocal.m_Z = (m_ScaleLocal.m_Z == 0.0f) ? Math::Utilities::Episilon : m_ScaleLocal.m_Z;

		UpdateTransform();
	}

	void Transform::Translate(const Vector3& delta)
	{
		if (!HasParent())
		{
			SetPositionLocal(m_PositionLocal + delta);
		}
		else
		{
			SetPositionLocal(m_PositionLocal + RetrieveParent()->RetrieveMatrix().Inverted() * delta);
		}
	}

	void Transform::Rotate(const Quaternion& delta)
	{
		///
	}

	Math::Vector3 Transform::RetrieveUpVector() const
	{
		//return RetrieveRotationLocal() * Vector3::Up;
	}

	Math::Vector3 Transform::RetrieveDownVector() const
	{
		//return RetrieveRotationLocal() * Vector3::Down;
	}

	Math::Vector3 Transform::RetrieveForwardVector() const
	{
		//return RetrieveRotationLocal() * Vector3::Forward;
	}

	Math::Vector3 Transform::RetrieveBackwardVector() const
	{
		//return RetrieveRotationLocal() * Vector3::Backward;
	}

	Math::Vector3 Transform::RetrieveRightVector() const
	{
		//return RetrieveRotationLocal() * Vector3::Right;
	}

	Math::Vector3 Transform::RetrieveLeftVector() const
	{
		//return RetrieveRotationLocal() * Vector3::Left;
	}

	// Sets a parent for this transform.
	void Transform::SetParent(Transform* newParent)
	{
		// If the new parent is null, it means that this should become a root transform.
		if (!newParent)
		{
			BecomeOrphan();
			return;
		}

		// Make sure the new parent is not this transform.
		if (RetrieveObjectID() == newParent->RetrieveObjectID())
		{
			return;
		}

		// Make sure that the new parent is different from the existing parent.
		if (HasParent())
		{
			if (RetrieveParent()->RetrieveObjectID() == newParent->RetrieveObjectID())
			{
				return;
			}
		}

		// If the new parent is a descendant of this transform.
		if (newParent->IsDescendantOf(this))
		{
			// If this parent already has a parent.
			if (this->HasParent())
			{
				// Assign the parent of this transform to the children.
				for (const auto& child : m_Children)
				{
					child->SetParent(RetrieveParent());
				}
			}
			else
			{
				// Make the children orphans.
				for (const auto& child : m_Children)
				{
					child->BecomeOrphan();
				}
			}
		}

		// Switch parent but keep a pointer to the old one.
		Transform* parentOld = m_Parent;
		m_Parent = newParent;
		if (parentOld)
		{
			parentOld->AcquireChildren(); // Update the old parent so it removes this child.
		}

		// Make the new parent aware of this transform/child.
		if (m_Parent)
		{
			m_Parent->AcquireChildren();
		}

		UpdateTransform();
	}

	void Transform::AddChild(Transform* child)
	{
		if (!child)
		{
			return;
		}

		if (RetrieveObjectID() == child->RetrieveObjectID())
		{
			return;
		}

		child->SetParent(this);
	}

	// Returns a child with the given index.
	Transform* Transform::RetrieveChildByIndex(const uint32_t index)
	{
		if (!HasChildren())
		{
			// AMETHYST_WARNING("%s has no children.", RetrieveEntityName().c_str());
			return nullptr;
		}

		// Prevent an out of vector bounds error.
		if (index >= RetrieveChildrenCount())
		{
			//AMETHYST_WARNING("There is no child with an index of \"%d\".", index);
			return nullptr;
		}

		return m_Children[index];
	}

	Transform* Transform::RetrieveChildByName(const std::string& entityName)
	{
		for (const auto& child : m_Children)
		{
			if (child->RetrieveEntityName() == entityName)
			{
				return child;
			}
		}

		return nullptr;
	}

	void Transform::AcquireChildren()
	{
		m_Children.clear();
		m_Children.shrink_to_fit();

		std::vector<std::shared_ptr<Entity>> entities = RetrieveEngineContext()->RetrieveSubsystem<World>()->RetrieveAllEntities();
		for (const std::shared_ptr<Entity>& entity : entities)
		{
			if (!entity)
			{
				continue;
			}

			// Retrieve the possible child.
			Transform* possibleChild = entity->RetrieveTransform();

			// If it doesn't have a parent, forget about it.
			if (!possibleChild->HasParent())
			{
				continue;
			}

			// If it's parent matches this transform.
			if (possibleChild->RetrieveParent()->RetrieveObjectID() == RetrieveObjectID())
			{
				// Welcome home son.
				m_Children.emplace_back(possibleChild);

				// Make the child do the same thing all over, essentially resolving the entire hierarchy.
				possibleChild->AcquireChildren();
			}
		}
	}

	bool Transform::IsDescendantOf(const Transform* transform) const
	{
		for (const Transform* child : transform->RetrieveChildren())
		{
			if (RetrieveObjectID() == child->RetrieveObjectID())
			{
				return true;
			}

			if (child->HasChildren()) // Do it all over again.
			{
				if (IsDescendantOf(child))
				{
					return true;
				}
			}
		}

		return false;
	}

	void Transform::RetrieveDescendants(std::vector<Transform*>* descendants)
	{
		for (Transform* child : m_Children)
		{
			descendants->emplace_back(child);

			if (child->HasChildren()) // Do it all over again.
			{
				child->RetrieveDescendants(descendants);
			}
		}
	}

	Matrix Transform::RetrieveParentTransformMatrix() const
	{
		return HasParent() ? RetrieveParent()->RetrieveMatrix() : Matrix::Identity;
	}

	// Make this transform have no parent.
	void Transform::BecomeOrphan()
	{
		// If there is no parent, the job is easy.
		if (!m_Parent)
		{
			return;
		}

		// Create a temporary reference to the parent.
		Transform* temporaryReference = m_Parent;

		// Delete the original reference.
		m_Parent = nullptr;

		// Update the transform without the parent now.
		UpdateTransform();

		// Make the parent search for children. That's an indirect way of making the parent "forget" about this child, since it won't be able to find it.
		if (temporaryReference)
		{
			temporaryReference->AcquireChildren();
		}
	}
}