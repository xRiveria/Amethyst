#pragma once
#include "ISubsystem.h"
#include <chrono>

namespace Amethyst
{
	class Context;

	enum class FPSLimitType
	{
		Unlocked,
		Fixed,
		FixedToMonitor
	};

	class Timer : public ISubsystem
	{
	public:
		Timer(Context* context);
		~Timer() = default;

		void OnUpdate(float deltaTime) override;

		//FPS
		void SetTargetFPS(double fpsIn);
		double RetrieveTargetFPS() const { return m_FPSTarget; }
		double RetrieveMinimumFPS() const { return m_FPSMinimum; }
		FPSLimitType RetrieveFPSLimitType();

		//Timer
		double RetrieveTimeInMilliseconds() const { return m_TimeInMilliseconds; }
		float RetrieveTimeInSeconds() const { return static_cast<float>(m_TimeInMilliseconds / 1000.0); }

		double RetrieveDeltaTimeInMilliseconds() const { return m_DeltaTimeInMilliseconds; }
		float RetrieveDeltaTimeInSeconds() const { static_cast<float>(m_DeltaTimeInMilliseconds / 1000.0); }

		double RetrieveDeltaTimeSmoothedInMilliseconds() { return m_DeltaTimeSmoothedInMilliseconds; }
		float RetrieveDeltaTimeSmoothedInSeconds() { return m_DeltaTimeSmoothedInMilliseconds / 1000.0; }

	private:
		//Frame Time
		std::chrono::high_resolution_clock::time_point m_TimeStart;
		std::chrono::high_resolution_clock::time_point m_TimeSleepStart;
		std::chrono::high_resolution_clock::time_point m_TimeSleepEnd;
		double m_TimeInMilliseconds = 0.0f;
		double m_DeltaTimeInMilliseconds = 0.0f;
		double m_DeltaTimeSmoothedInMilliseconds = 0.0f;
		double m_SleepOverhead = 0.0f;

		//FPS
		double m_FPSMinimum = 30.0f;
		double m_FPSMaximum = 1000.0f;
		double m_FPSTarget = m_FPSMaximum;
		bool m_IsUserSelectedFPSTarget = false;
	};
}