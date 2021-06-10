#pragma once
#include "IComponent.h"
#include <memory>
#include "../RHI/RHI_Utilities.h"
#include "../RHI/RHI_Viewport.h"
#include "../../Math/Matrix.h"
#include "../../Math/Vector2.h"
#include "../../Math/Rectangle.h"
#include "../../Math/Ray.h"
#include "../../Math/Frustum.h"
#include "../../Math/BoundingBox.h"

namespace Amethyst
{
	class Entity;
	class Model;
	class Renderable;
	class Input;
	class Renderer;

	enum ProjectionType
	{
		Projection_Perspective,
		Projection_Orthographic
	};

	class Camera : public IComponent
	{
	public:
		Camera(Context* engineContext, Entity* entity, uint32_t componentID = 0);
		~Camera() = default;

		// === IComponent ===
		void OnInitialize() override;
		void OnUpdate(float deltaTime) override;
		/// Serialize
		/// Deserialize
		
		// Matrices
		const Math::Matrix& RetrieveViewMatrix() const { return m_ViewMatrix; }
		const Math::Matrix& RetrieveProjectionMatrix() const { return m_ProjectionMatrix; }
		const Math::Matrix& RetrieveViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
		
		// Raycasting
		/// const Math::Ray& RetrievePickingRay() const { return m_Ray; } // Returns the ray the camera uses to do picking.

		// Picks the nearest entity under the mouse cursor.
		bool PickScreenEntity(std::shared_ptr<Entity>& entity);

		// Converts a world point to a screen point.
		/// Math::Vector2 WorldToScreenPoint(const Math::Vector3& positionWorld) const;

		// Converts a screen point to world point.
		/// Math::Vector2 ScreenToWorldPoint(const Math::Vector2& positionScreen) const;

		// Converts a world bounding box to a screen rectangle.
		/// Math::Rectangle WorldToScreenRectangle(const Math::BoundingBox& boundingBox) const;
		
		/// Retrieve/Set Aperture
		/// Retrieve/Set Shutter
		/// Retrieve/Set ISO
		/// Retrieve Ev100
		/// Retrieve Exposure
		
		// Planes/Projection
		void SetNearPlane(float nearPlane);
		void SetFarPlane(float farPlane);
		void SetProjectionType(ProjectionType projection);
		float RetrieveNearPlane() const { return m_NearPlane; }
		float RetrieveFarPlane() const { return m_FarPlane; }
		ProjectionType RetrieveProjectionType() const { return m_ProjectionType; }

		// FOV
		/// FOV Stuff
		const RHI_Viewport& RetrieveViewport() const;

		// Misc
		const Math::Vector4& RetrieveClearColor() const { return m_ClearColor; }
		void SetClearColor(const Math::Vector4& color) { m_ClearColor = color; }
		bool IsInViewFrustrum(Renderable* renderable) const;
		bool IsInViewFrustrum(const Math::Vector3& center, const Math::Vector3& extents) const;
		bool RetrieveFPSControlEnabled() const { return m_FPS_ControlEnabled; }
		void SetFPSControlEnabled(const bool isEnabled) { m_FPS_ControlEnabled = isEnabled; }
		bool IsFPSControlled() const { return m_FPS_ControlAssumed; }

		Math::Matrix ComputeViewMatrix() const;
		Math::Matrix ComputeProjectionMatrix(const bool reverseZ, const float nearPlane = 0.0f, const float farPlane = 0.0f);

	private:
		void FPSControl(float deltaTime);

	private:
		/// Aperture
		/// Shutter Speed
		/// ISO
		float m_NearPlane = 0.3f;
		float m_FarPlane = 1000.0f;
		ProjectionType m_ProjectionType = ProjectionType::Projection_Perspective;
		Math::Vector4 m_ClearColor = Math::Vector4(0.396f, 0.611f, 0.937f, 1.0f); // A nice cornflower blue.
		Math::Matrix m_ViewMatrix = Math::Matrix::Identity;
		Math::Matrix m_ProjectionMatrix	= Math::Matrix::Identity;
		Math::Matrix m_ViewProjectionMatrix = Math::Matrix::Identity;
		Math::Vector3 m_Position = Math::Vector3::Zero;
		Math::Quaternion m_Rotation = Math::Quaternion::Identity;

		/// FOV Horizontal Radius
		bool m_IsDirty = false;
		bool m_FPS_ControlEnabled = true;
		bool m_FPS_ControlAssumed = false;
		bool m_FPS_ControlCursorHidden = false;
		Math::Vector2 m_MouseLastPosition = Math::Vector2::Zero;

		// Camera Speed
		Math::Vector2 m_MouseRotation = Math::Vector2::Zero;
		Math::Vector3 m_MovementSpeed = Math::Vector3::Zero;
		float m_MovementSpeedMinimum = 0.5f;
		float m_MovementSpeedMaximum = 5.0f;
		float m_MovementAcceleration = 1000.0f;
		float m_MovementDrag = 10.0f;
		Math::Vector2 m_MouseSmoothed = Math::Vector2::Zero;
		float m_MouseSensitivity = 0.2f;
		float m_MouseSmoothing = 0.5f;
		RHI_Viewport m_LastKnownViewport;
		/// Math::Ray m_Ray;
		Math::Frustum m_Frustrum;

		// Dependencies
		Renderer* m_Renderer = nullptr;
		Input* m_Input = nullptr;
	};
}
