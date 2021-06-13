#include "Amethyst.h"
#include "Camera.h"
#include "Transform.h"
#include "Renderable.h"
#include "../Display/Display.h"
#include "../Runtime/ECS/Entity.h"
#include "../Runtime/ECS/World.h"
#include "../Input/Input.h"
#include "../Rendering/Renderer.h"

namespace Amethyst
{
	Camera::Camera(Context* engineContext, Entity* entity, uint32_t componentID /*= 0*/) : IComponent(engineContext, entity, componentID)
	{
		m_Renderer = m_Context->RetrieveSubsystem<Renderer>();
		m_Input = m_Context->RetrieveSubsystem<Input>();
	}

	void Camera::OnInitialize()
	{
		m_ViewMatrix = ComputeViewMatrix();
		m_ProjectionMatrix = ComputeProjectionMatrix(m_Renderer->RetrieveRendererOption(RendererOption::Render_ReverseZ));
		m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
	}

	void Camera::OnUpdate(float deltaTime)
	{
		const RHI_Viewport& currentViewport = m_Renderer->RetrieveViewport();
		if (m_LastKnownViewport != currentViewport)
		{
			m_LastKnownViewport = currentViewport;
			m_IsDirty = true;
		}

		// Dirty check.
		if (m_Position != RetrieveTransform()->RetrievePosition()) /*|| m_Rotation != RetrieveTransform()->RetrieveRotation()*/
		{
			m_Position = RetrieveTransform()->RetrievePosition();
			m_Rotation = RetrieveTransform()->RetrieveRotation();
			m_IsDirty = true;
		}

		/// if (m_FPS_ControlEnabled)
		/// {
		///	FPSControl(deltaTime);
		/// }

		if (!m_IsDirty)
		{
			return;
		}

		m_ViewMatrix = ComputeViewMatrix();
		m_ProjectionMatrix = ComputeProjectionMatrix(m_Renderer->RetrieveRendererOption(RendererOption::Render_ReverseZ));
		m_ViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
		m_Frustrum = Math::Frustum(RetrieveViewMatrix(), RetrieveProjectionMatrix(), m_Renderer->RetrieveRendererOption(RendererOption::Render_ReverseZ) ? RetrieveNearPlane() : RetrieveFarPlane());

		m_IsDirty = false;
	}

	void Camera::SetNearPlane(const float nearPlane)
	{
		m_NearPlane = Math::Utilities::Max(0.01f, nearPlane);
		m_IsDirty = true;
	}

	void Camera::SetFarPlane(const float farPlane)
	{
		m_FarPlane = farPlane;
		m_IsDirty = true;
	}

	void Camera::SetProjectionType(const ProjectionType projectionType)
	{
		m_ProjectionType = projectionType;
		m_IsDirty = true;
	}

	float Camera::RetrieveFOVHorizontalInDegrees() const
	{
		return Math::Utilities::RadiansToDegrees(m_FOVHorizontalInRadians);
	}

	void Camera::SetFOVHorizontalInDegrees(float fieldOfView)
	{
		m_FOVHorizontalInRadians = Math::Utilities::DegreesToRadians(fieldOfView);
		m_IsDirty = true;
	}

	const RHI_Viewport& Camera::RetrieveViewport() const
	{
		return m_Renderer ? m_Renderer->RetrieveViewport() : RHI_Viewport::Undefined;
	}

	bool Camera::IsInViewFrustrum(Renderable* renderable) const
	{
		const Math::BoundingBox& boundingBox = renderable->RetrieveAABB();
		const Math::Vector3 center = boundingBox.RetrieveCenter();
		const Math::Vector3 extents = boundingBox.RetrieveExtents();

		return m_Frustrum.IsVisible(center, extents);
	}

	bool Camera::IsInViewFrustrum(const Math::Vector3& center, const Math::Vector3& extents) const
	{
		return m_Frustrum.IsVisible(center, extents);
	}

	Math::Matrix Camera::ComputeViewMatrix() const
	{
		const Math::Vector3 positionVector = RetrieveTransform()->RetrievePosition(); // Camera Position
		Math::Vector3 lookAtVector = RetrieveTransform()->RetrieveRotation() * Math::Vector3::Forward;
		const auto upVector = RetrieveTransform()->RetrieveRotation() * Math::Vector3::Up;

		// Offset lookAt by current position.
		lookAtVector += positionVector;

		// Compute View Matrix
		return Math::Matrix::CreateLookAtMatrix(positionVector, lookAtVector, upVector);
	}

	Math::Matrix Camera::ComputeProjectionMatrix(const bool reverseZ, const float nearPlane /*= 0.0f*/, const float farPlane /*= 0.0f*/)
	{
		float _nearPlane = nearPlane != 0 ? nearPlane : m_NearPlane;
		float _farPlane = farPlane != 0 ? farPlane : m_FarPlane;

		if (reverseZ)
		{
			const float temporaryNearPlane = _nearPlane;
			_nearPlane = _farPlane;
			_farPlane = temporaryNearPlane;
		}

		if (m_ProjectionType == ProjectionType::Projection_Perspective)
		{
			return Math::Matrix::CreatePerspectiveMatrix(RetrieveFOVHorizontalInRadians(), RetrieveViewport().RetrieveAspectRatio(), _nearPlane, _farPlane);
		}

		else if (m_ProjectionType == ProjectionType::Projection_Orthographic)
		{
			return Math::Matrix::CreateOrthographic(RetrieveViewport().m_Width, RetrieveViewport().m_Height, _nearPlane, _farPlane);
		}

		return Math::Matrix::Identity;
	}
}