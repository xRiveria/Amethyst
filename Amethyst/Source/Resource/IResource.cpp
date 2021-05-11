#include "Amethyst.h"
#include "IResource.h"

namespace Amethyst
{
	IResource::IResource(Context* context, ResourceType resourceType)
	{
		m_Context = context;
		m_ResourceType = resourceType;
		m_LoadState = LoadState::Idle;
	}

	template<typename T>
	inline constexpr ResourceType IResource::TypeToEnum()
	{
		return ResourceType::Unknown;
	}

	template<typename T>
	inline constexpr void ValidateResourceType() 
	{ 
		static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource."); 
	}

	//Explict template instantiation.
	#define INSTANTIATE_TO_RESOURCE_TYPE(T, enumT) template<> ResourceType IResource::TypeToEnum<T>() { ValidateResourceType<T>(); return enumT; }

	//To add a new resource to the engine, simply register it here.
	
}