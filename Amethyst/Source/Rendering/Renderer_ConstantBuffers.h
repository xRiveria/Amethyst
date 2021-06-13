#pragma once
#include "../Runtime/Math/Vector2.h"
#include "../Runtime/Math/Vector3.h"
#include "../Runtime/Math/Matrix.h"

namespace Amethyst
{
	/*
		4 byte (32 bit) aligned. Equals are created to prevent unnecessary updates to the buffer.
	*/

	// Low Frequency Buffer - Updates once per frame.
	struct BufferFrame
	{
		Math::Matrix m_View;
		Math::Matrix m_Projection;
		Math::Matrix m_ProjectionOrthographic;

		Math::Matrix m_ViewProjection;
		Math::Matrix m_ViewProjectionOrthographic;
		Math::Matrix m_ViewProjectionInversed;
		Math::Matrix m_ViewProjectionUnjittered; // For temporal image effects, the camera that is currently rendering needs to be slightly offset from the default projection (hence, the camera is "jittered").
		Math::Matrix m_ViewProjectionPrevious;

		float m_DeltaTime;
		float m_Time;
		uint32_t m_FrameNumber;
		/// Aperture
		/// Shutter Speed
		/// ISO
		float m_CameraNear;

		float m_CameraFar;
		Math::Vector3 m_CameraPosition;
		/// Bloom Intensity

		/// Sharpen Strength
		Math::Vector3 m_CameraDirection;
		float m_Padding;

		/// Gamma
		/// Tone Mapping
		/// Directional Light Intensity
		/// Shadow Resolution
		
		Math::Vector2 m_ResolutionRender;
		Math::Vector2 m_ResolutionOutput;

		/// Jitter Offset
		/// Fog
		uint32_t m_Options;
		float m_Padding1;
		float m_Padding2;
		float m_Padding3;

		void SetBit(const bool isSet, const uint32_t bit)
		{
			m_Options = isSet ? (m_Options |= bit) : (m_Options & ~bit);
		}

		bool operator==(const BufferFrame& bufferFrame) const
		{
			return
				m_View == bufferFrame.m_View &&
				m_Projection == bufferFrame.m_Projection &&
				m_ProjectionOrthographic == bufferFrame.m_ProjectionOrthographic &&
				m_ViewProjection == bufferFrame.m_ViewProjection &&
				m_ViewProjectionOrthographic == bufferFrame.m_ViewProjectionOrthographic &&
				m_ViewProjectionInversed == bufferFrame.m_ViewProjectionInversed &&
				m_ViewProjectionUnjittered == bufferFrame.m_ViewProjectionUnjittered &&
				m_ViewProjectionPrevious == bufferFrame.m_ViewProjectionPrevious &&
				m_DeltaTime == bufferFrame.m_DeltaTime &&
				m_Time == bufferFrame.m_Time &&
				m_FrameNumber == bufferFrame.m_FrameNumber &&
				m_CameraNear == bufferFrame.m_CameraNear &&
				m_CameraFar == bufferFrame.m_CameraFar &&
				m_CameraPosition == bufferFrame.m_CameraPosition &&
				m_CameraDirection == bufferFrame.m_CameraDirection &&
				m_ResolutionRender == bufferFrame.m_ResolutionRender &&
				m_ResolutionOutput == bufferFrame.m_ResolutionOutput &&
				m_Options == bufferFrame.m_Options;
		};

		bool operator !=(const BufferFrame& bufferFrame) const { return !(*this == bufferFrame); }
	};

	// High Frequency - Updates like Crazy. 
	struct BufferUber
	{
		Math::Matrix m_Transform;
		Math::Matrix m_TransformPrevious;

		Math::Vector4 m_Color;
		
		Math::Vector3 m_TransformAxis;
		float m_Padding;
		/// Blur Sigma
		
		/// Blur Direction
		Math::Vector2 m_Resolution;
		float m_Padding1;
		float m_Padding2;
		
		/// Material Albedo
		
		/// Material Tiling UV
		/// Material Offset UV
		
		/// Roughness
		/// Metallic
		/// Normal
		/// Height
		
		/// Material ID
		/// MIP Index
		/// Is Transparent Pass
		/// Padding
		/// 
		
		bool operator==(const BufferUber& bufferUber) const
		{
			return
				m_Transform == bufferUber.m_Transform &&
				m_TransformPrevious == bufferUber.m_TransformPrevious &&
				m_Color == bufferUber.m_Color &&
				m_TransformAxis == bufferUber.m_TransformAxis &&
				m_Resolution == bufferUber.m_Resolution;
		};

		bool operator !=(const BufferUber& bufferUber) const { return !(*this == bufferUber); }
	};
}