#pragma once
#include "ISubsystem.h"
#include <vector>
#include "Engine.h"

namespace Amethyst
{
	class Engine;

	enum class TickType
	{
		Variable,
		Smoothed
	};

	struct _Subsystem
	{
		_Subsystem(const std::shared_ptr<ISubsystem> subsystem, TickType tickType)
		{
			m_SubsystemPointer = subsystem;
			m_TickType = tickType;
		}

		std::shared_ptr<ISubsystem> m_SubsystemPointer;
		TickType m_TickType;
	};

	class Context
	{
	public:
		Context() = default;

		~Context()
		{
			//Loop in reverse registration order to avoid dependency conflicts.
			for (size_t i = m_Subsystems.size() - 1; i > 0; i--)
			{
				m_Subsystems[i].m_SubsystemPointer.reset();
			}

			m_Subsystems.clear();
		}

		//Register a subsystem.
		template<typename T>
		void RegisterSubsystem(TickType tickType = TickType::Variable)
		{
			ValidateSubsystemType<T>();
			m_Subsystems.emplace_back(std::make_shared<T>(this), tickType);
		}

		//Initialize Subsystems.
		bool InitializeSubsystems()
		{
			bool result = true;
			for (const _Subsystem& subsystem : m_Subsystems)
			{
				if (!subsystem.m_SubsystemPointer->InitializeSubsystem())
				{
					//Log Error.
					result = false;
				}
			}

			return result;
		}

		//Tick
		void OnUpdate(TickType tickType, float deltaTime = 0.0f)
		{
			for (const _Subsystem& subsystem : m_Subsystems)
			{
				if (subsystem.m_TickType != tickType)
				{
					continue;
				}

				subsystem.m_SubsystemPointer->OnUpdate(deltaTime);
			}
		}

		//Retrieve a subsystem.
		template<typename T>
		T* RetrieveSubsystem() const
		{
			ValidateSubsystemType<T>();

			for (const _Subsystem& subsystem : m_Subsystems)
			{
				if (subsystem.m_SubsystemPointer)
				{
					if (typeid(T) == typeid(*subsystem.m_SubsystemPointer))
					{
						return static_cast<T*>(subsystem.m_SubsystemPointer.get());
					}
				}
			}

			return nullptr;
		}

	public:
		Engine* m_Engine = nullptr;

	private:
		std::vector<_Subsystem> m_Subsystems;
	};
}