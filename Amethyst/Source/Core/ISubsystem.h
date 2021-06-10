#pragma once
#include <memory>

namespace Amethyst
{
	class Context;

	class ISubsystem : public std::enable_shared_from_this<ISubsystem>
	{
	public:
		ISubsystem(Context* context) { m_EngineContext = context; }
		virtual ~ISubsystem() = default;
		virtual bool InitializeSubsystem() { return true; }
		virtual void OnUpdate(float deltaTime) {}

		template<typename T>
		std::shared_ptr<T> RetrieveSharedPointer() { return std::dynamic_pointer_cast<T>(shared_from_this); }

	protected:
		Context* m_EngineContext;
	};

	template<typename T>
	constexpr void ValidateSubsystemType()
	{
		static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem.");
	}
}