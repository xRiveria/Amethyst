#pragma once
#include <chrono>

namespace Amethyst
{
	class Stopwatch //RAII
	{
	public:
		Stopwatch()
		{
			Start();
		}

		~Stopwatch() = default;

		void Start()
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		float RetrieveElapsedTimeInSeconds() const
		{
			const std::chrono::duration<double, std::milli> milliSeconds = std::chrono::high_resolution_clock::now() - m_Start;
			return static_cast<float>(milliSeconds.count() / 1000.0f);
		}

		float RetrieveElapsedTimeInMilliseconds() const
		{
			const std::chrono::duration<double, std::milli> milliSeconds = std::chrono::high_resolution_clock::now() - m_Start;
			return static_cast<float>(milliSeconds.count());
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};
}