#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <unordered_map>
#include <functional>
#include "../Runtime/Log/Log.h"
#include "../Core/ISubsystem.h"

namespace Amethyst
{
	class Task
	{
	public:
		typedef std::function<void()> FunctionType;

		Task(FunctionType&& function) { m_Function = std::forward<FunctionType>(function); }
		void ExecuteTask() 
		{ 
			m_IsExecuting = true; 
			m_Function(); 
			m_IsExecuting = false; 
		}
		bool IsExecuting() const { return m_IsExecuting; }

	private:
		bool m_IsExecuting = false;
		FunctionType m_Function;
	};

	class Threading : public ISubsystem
	{
	public:
		Threading(Context* context);
		~Threading();

		//Add a task.
		template<typename Function>
		void AddTask(Function&& function)
		{
			if (m_Threads.empty())
			{
				AMETHYST_WARNING("No avaliable threads. Function will execute in the same thread.");
				function();
				return;
			}

			//Lock tasks mutex.
			std::unique_lock<std::mutex> taskMutex(m_TasksMutex);

			//Save the task.
			m_Tasks.push_back(std::make_shared<Task>(std::bind(std::forward<Function>(function))));

			//Unlock the mutex.
			taskMutex.unlock();

			//Wake up a thread to handle the task.
			m_ConditionVariable.notify_one();
		}

		//Retrieve the number of threads being used.
		uint32_t RetrieveThreadCount() const { return m_ThreadCount; }
		//Retrieve the maximum number of threads the hardware supports.
		uint32_t RetrieveThreadCountSupported() const { return m_ThreadCountSupported; }
		//Retrieve the number of free threads.
		uint32_t RetrieveThreadsAvaliable() const;
		//Returns true if at least one task is running.
		bool AreTasksRunning() const { return RetrieveThreadsAvaliable() != RetrieveThreadCount(); }
		//Wait for all executing (and queued if requested) tasks to finish.
		void FlushTasks(bool removeQueued = false);

	private:
		void ThreadLoop(); //This function is invoked by the threads.

	private:
		uint32_t m_ThreadCount = 0; //Does not include the main thread.
		uint32_t m_ThreadCountSupported = 0;
		std::unordered_map<std::thread::id, std::string> m_ThreadNames;

		std::deque<std::shared_ptr<Task>> m_Tasks;
		std::vector<std::thread> m_Threads;
		std::condition_variable m_ConditionVariable;
		std::mutex m_TasksMutex;
		bool m_Stopping;
	};
}