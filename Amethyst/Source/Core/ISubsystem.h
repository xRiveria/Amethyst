#pragma once
#include <memory>

namespace Amethyst
{
	//Serves purely as an interface. No data storage is to be allocated here.

	class ISubsystem : public std::enable_shared_from_this<ISubsystem>
	{
	public:
		ISubsystem() {}
		virtual ~ISubsystem() = default;
		virtual bool InitializeSubsystem() { return true; }
		virtual void OnUpdate(float deltaTime) {}

		template<typename T>
		std::shared_ptr<T> RetrieveSharedPointer() { return std::dynamic_pointer_cast<T>(shared_from_this); }
	};

	template<typename T>
	constexpr void ValidateSubsystemType()
	{
		static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem.");
	}
}