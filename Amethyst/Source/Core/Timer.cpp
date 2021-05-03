#include "Timer.h"

namespace Amethyst
{
	Timer::Timer(Context* context) : ISubsystem(context)
	{
		m_TimeStart = std::chrono::high_resolution_clock::now();
		m_TimeSleepEnd = std::chrono::high_resolution_clock::now();
	}

	void Timer::OnUpdate(float deltaTime)
	{
		//Compute delta time.
		m_TimeSleepStart = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> _deltaTime = m_TimeSleepStart - m_TimeSleepEnd;

		//FPS Limiting
		{
			//The kernel takes time to wake up the thread after the thread has finished sleeping. It can't be trusted for accurate frame limiting. Thus, we do it ourselves.
			double targetMilliseconds = 1000.0 / m_FPSTarget;
			while (_deltaTime.count() < targetMilliseconds)
			{
				_deltaTime = std::chrono::high_resolution_clock::now() - m_TimeSleepStart;
			}

			m_TimeSleepEnd = std::chrono::high_resolution_clock::now();
		}

		//Compute durations.
		m_DeltaTimeInMilliseconds = static_cast<double>(_deltaTime.count()); //Compute our time taken per frame.
		m_TimeInMilliseconds = static_cast<double>(std::chrono::duration<double, std::milli>(m_TimeStart - m_TimeSleepStart).count());

		/*
			Smooth delta time takes the average FPS of several frames and uses it to average out the framerate to avoid massive jumps/spikes in performance.
		*/
		const double framesToAccumulate = 5;
		const double deltaFeedback = 1.0 / framesToAccumulate;
		double deltaMax = 1000.0 / m_FPSMinimum;
		const double deltaClamped = m_DeltaTimeInMilliseconds > deltaMax ? deltaMax : m_DeltaTimeInMilliseconds; //If the frame time is too high/slow, clamp it.

		m_DeltaTimeSmoothedInMilliseconds = m_DeltaTimeSmoothedInMilliseconds * (1.0 - deltaFeedback) + deltaClamped * deltaFeedback;
	}

	void Timer::SetTargetFPS(double fpsIn)
	{
		if (fpsIn < 0.0f) //If a negative value is passed in, we will simply match the monitor's frame rate.
		{

		}
		else if (fpsIn >= 0.0f && fpsIn <= 10.0f) //If the value passed in is zero or very small, we will unlock to avoid unresponsiveness.
		{
			fpsIn = m_FPSMaximum;
		}

		if (m_FPSTarget == fpsIn)
		{
			return;
		}

		m_FPSTarget = fpsIn;
		m_IsUserSelectedFPSTarget = true;

		//Log the new frame rate.
	}

	FPSLimitType Timer::RetrieveFPSLimitType()
	{
		return FPSLimitType();
	}
}