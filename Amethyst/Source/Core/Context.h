#pragma once
#include "ISubsystem.h"
#include "../Runtime/Log/Log.h"
#include "AmethystDefinitions.h"

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
		void OnInitialize()
		{
			std::vector<uint32_t> failedSubsystems;

			// Initialize Subsystems
			for (uint32_t i = 0; i < static_cast<uint32_t>(m_Subsystems.size()); i++)
			{
				if (!m_Subsystems[i].m_SubsystemPointer->OnInitialize())
				{
					failedSubsystems.emplace_back(i);
					AMETHYST_ERROR("Failed to initialize %s", typeid(*m_Subsystems[i].m_SubsystemPointer).name()); // Note: Calling * on a shared pointer returns the result of dereferencing the stored (raw) pointer.
				}
			}

			// Remove the ones that failed. 
			for (const uint32_t failedSubsystemIndex : failedSubsystems)
			{
				m_Subsystems.erase(m_Subsystems.begin() + failedSubsystemIndex);
			}
		}

		// Pre-Tick
		void OnPreUpdate()
		{
			for (const _Subsystem& subsystem : m_Subsystems)
			{
				subsystem.m_SubsystemPointer->OnPreUpdate();
			}
		}

		// Tick
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

		// Post-Tick
		void OnPostUpdate()
		{
			for (const _Subsystem& subsystem : m_Subsystems)
			{
				subsystem.m_SubsystemPointer->OnPostUpdate();
			}
		}

		// Shutdown
		void OnShutdown()
		{
			for (const _Subsystem& subsystem : m_Subsystems)
			{
				subsystem.m_SubsystemPointer->OnShutdown();
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