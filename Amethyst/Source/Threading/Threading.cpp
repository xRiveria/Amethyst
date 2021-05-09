#include "Amethyst.h"
#include "Threading.h"

namespace Amethyst
{
	Threading::Threading(Context* context) : ISubsystem(context)
	{
		m_Stopping = false;
		m_ThreadCountSupported = std::thread::hardware_concurrency();
		m_ThreadCount = m_ThreadCountSupported - 1; //Not including our main thread.
		m_ThreadNames[std::this_thread::get_id()] = "Main";

		for (uint32_t i = 0; i < m_ThreadCount; i++)
		{
			m_Threads.emplace_back(std::thread(&Threading::ThreadLoop, this));
			m_ThreadNames[m_Threads.back().get_id()] = "Worker_" + std::to_string(i);
		}

		AMETHYST_INFO("%d threads have been created.", m_ThreadCount);
	}

	Threading::~Threading()
	{
		FlushTasks(); //Execute all remaining tasks.

		//Put unique lock on our task mutex.
		std::unique_lock<std::mutex>(m_TasksMutex);

		//Set termination flag to true.
		m_Stopping = true;

		//Unlock the mutex.
		m_TasksMutex.unlock();

		//Wake up all threads.
		m_ConditionVariable.notify_all();

		//Join all threads.
		for (std::thread& thread : m_Threads)
		{
			thread.join();
		}

		//Empty worker threads.
		m_Threads.clear();
	}

	uint32_t Threading::RetrieveThreadsAvaliable() const
	{
		uint32_t avaliableThreads = m_ThreadCount;

		for (const std::shared_ptr<Task>& task : m_Tasks)
		{
			avaliableThreads -= task->IsExecuting() ? 1 : 0;
		}

		return avaliableThreads;
	}

	void Threading::FlushTasks(bool removeQueued)
	{
		//Clear any queued tasks.
		if (removeQueued)
		{
			m_Tasks.clear();
		}

		//If there are any tasks currently running, we shall wait for them to complete.
		while (AreTasksRunning())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	void Threading::ThreadLoop()
	{
		std::shared_ptr<Task> task;

		while (true)
		{
			//Lock the tasks mutex.
			std::unique_lock<std::mutex> taskMutex(m_TasksMutex);

			//Check condition on notification.
			m_ConditionVariable.wait(taskMutex, [this] { return !m_Tasks.empty() || m_Stopping; }); //Return true if we have tasks or if threading is to be stopped.

			//If m_Stopping is true, its time to shut everything down.
			if (m_Stopping && m_Tasks.empty())
			{
				return;
			}

			//Else, we have tasks to complete.
			task = m_Tasks.front();

			//Remove said task from the queue.
			m_Tasks.pop_front();

			//Unlock the mutex.
			taskMutex.unlock();

			//Execute the task.
			task->ExecuteTask();
		}
	}
}