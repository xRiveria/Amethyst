#pragma once
#include <functional>

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
}