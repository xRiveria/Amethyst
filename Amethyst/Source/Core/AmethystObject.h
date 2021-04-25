#pragma once
#include <memory>
#include <string>

namespace Amethyst
{
	extern uint32_t g_ObjectID;

	class AmethystObject
	{
	public:
		AmethystObject();

		//Name
		const std::string& RetrieveObjectName() const { return m_Name; }

		//ID
		const uint32_t RetrieveObjectID() const { return m_ID; }
		void SetObjectID(const uint32_t objectID) { m_ID = objectID; }
		static uint32_t GenerateObjectID() { return ++g_ObjectID; }

	protected:
		std::string m_Name;
		uint32_t m_ID = 0;
	};
}
