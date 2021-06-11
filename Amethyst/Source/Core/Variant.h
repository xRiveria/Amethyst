#pragma once
#include "../Runtime/Math/Vector2.h"
#include "../Runtime/Math/Vector3.h"
#include "../Runtime/Math/Vector4.h"
#include "../Runtime/Math/Quaternion.h"
#include "../Runtime/Math/Matrix.h"
#include <variant>
#include "AmethystDefinitions.h"

namespace Amethyst
{
	class Entity;
}
typedef union SDL_Event SDL_Event;

#define _VARIANT_TYPES										\
		char,												\
		unsigned char,										\
		int,												\
		uint32_t,											\
		bool,												\
		float,												\
		double,												\
		void*,												\
		Amethyst::Entity*,									\
		std::shared_ptr<Amethyst::Entity>,					\
		std::weak_ptr<Amethyst::Entity>,					\
		std::vector<std::weak_ptr<Amethyst::Entity>>,		\
		std::vector<std::shared_ptr<Amethyst::Entity>>,		\
		Amethyst::Math::Vector2,							\
		Amethyst::Math::Vector3,							\
		Amethyst::Math::Vector4,							\
		Amethyst::Math::Matrix,								\
		Amethyst::Math::Quaternion,							\
		SDL_Event*

#define VARIANT_TYPES std::variant<_VARIANT_TYPES>
typedef std::variant<_VARIANT_TYPES, VARIANT_TYPES> VariantInternal;

namespace Amethyst
{
	class Variant
	{
	public:
		//Default
		Variant() = default;
		~Variant() = default;

		//Copy Constructor 1
		Variant(const Variant& variant) { m_Variant = variant.RetrieveVariantRaw(); }
		//Copy Constructor 2
		template <typename T, typename = std::enable_if<!std::is_same<T, Variant>::value>>	
		Variant(T value) { m_Variant = value; }

		//Assignment Operator 1
		Variant& operator=(const Variant& otherVariant);
		//Assignment Operator 2
		template <typename T, typename = std::enable_if<!std::is_same<T, Variant>::value>>	//Only if the underlying variant is the same.
		Variant& operator=(T value) { return m_Variant = value; }

		const VariantInternal& RetrieveVariantRaw() const { return m_Variant; }

		template<typename T>
		inline const T& RetrieveValue() const { return std::get<T>(m_Variant); }
 
	private:
		VariantInternal m_Variant;
	};
}
