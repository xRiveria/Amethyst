#include "Amethyst.h"
#include "Resource.h"

namespace Amethyst
{
	Resource::Resource(const ResourceType resourceType)
	{
		m_ResourceType = resourceType;
		m_ResourceLoadState = LoadState::Idle;
	}

	template<typename T>
	inline constexpr ResourceType Resource::TypeToEnum()
	{
		return ResourceType::Unknown;
	}

	template<typename T>
	inline constexpr void ValidateResourceType() { static_assert(std::is_base_of<Resource, T>::value, "Provided type does not implement the Resource interface."); }
}