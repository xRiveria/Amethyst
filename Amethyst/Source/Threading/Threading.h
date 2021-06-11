#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <unordered_map>
#include "../Runtime/Log/Log.h"
#include "../Core/ISubsystem.h"
#include "Task.h"

namespace Amethyst
{
	class Threading : public ISubsystem
	{
	public:
		Threading(Context* context);
		~Threading();

		// Add a task.
		template<typename Function>
		void AddTask(Function&& function)
		{
			if (m_Threads.empty())
			{
				AMETHYST_WARNING("No avaliable threads. Function will execute in the same thread.");
				function();
				return;
			}

			// Lock tasks mutex.
			std::unique_lock<std::mutex> taskMutex(m_TasksMutex);

			// Save the task.
			m_Tasks.push_back(std::make_shared<Task>(std::bind(std::forward<Function>(function))));

			// Unlock the mutex.
			taskMutex.unlock();

			// Wake up a thread to handle the task.
			m_ConditionVariable.notify_one();
		}

		///
		// Adds a task which is a loop and executes chunks of it in parallel.
		template<typename Function>
		void AddTaskLoop(Function&& function, uint32_t range)
		{
			uint32_t avaliableThreads = RetrieveThreadsAvaliable();
			std::vector<bool> tasksComplete = std::vector<bool>(avaliableThreads, false); // Creates a vector with a count of all currently avaliable threads.
			const uint32_t taskCount = avaliableThreads + 1; // Plus one for the current thread.

			uint32_t start = 0;
			uint32_t end = 0;

			for (uint32_t i = 0; i < avaliableThreads; i++)
			{
				start = (range / taskCount) * i;
				end = start + (range / taskCount);

				// Kick off task.
				AddTask([&function, &tasksComplete, i, start, end] { function(start, end); tasksComplete[i] = true; });
			}

			// Complete the last task in the current thread.
			function(end, range);

			// Wait till the threads are done.
			uint32_t tasks = 0;
			while (tasks != tasksComplete.size())
			{
				tasks = 0;
				for (const bool jobDone : tasksComplete)
				{
					tasks += jobDone ? 1 : 0;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Set current thread to sleep as we wait for the tasks to complete.
			}
		}

		// Retrieve the number of threads being used.
		uint32_t RetrieveThreadCount() const { return m_ThreadCount; }
		// Retrieve the maximum number of threads the hardware supports.
		uint32_t RetrieveThreadCountSupported() const { return m_ThreadCountSupported; }
		// Retrieve the number of free threads.
		uint32_t RetrieveThreadsAvaliable() const;
		// Returns true if at least one task is running.
		bool AreTasksRunning() const { return RetrieveThreadsAvaliable() != RetrieveThreadCount(); }
		//W ait for all executing (and queued if requested) tasks to finish.
		void FlushTasks(bool removeQueued = false);

	private:
		void ThreadLoop(); // This function is invoked by the threads.

	private:
		uint32_t m_ThreadCount = 0; //Does not include the main thread.
		uint32_t m_ThreadCountSupported = 0;
		std::unordered_map<std::thread::id, std::string> m_ThreadNames;

		std::deque<std::shared_ptr<Task>> m_Tasks;
		std::vector<std::thread> m_Threads;
		std::condition_variable m_ConditionVariable;
		std::mutex m_TasksMutex;
		bool m_IsStopping;
	};
}