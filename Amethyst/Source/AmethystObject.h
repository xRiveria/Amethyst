#pragma once
#include <memory>
#include <string>

namespace Amethyst
{
	//Global
	static uint32_t s_GlobalID = 0;

	class AmethystObject
	{
	public:
		AmethystObject()
		{
			m_ObjectID = GenerateObjectID();
		}

		//Name
		const std::string& RetrieveObjectName() const { return m_ObjectName; }

		//ID
		const uint32_t RetrieveObjectID() const { return m_ObjectID; }
		void SetObjectID(const uint32_t objectID) { m_ObjectID = objectID; }
		uint32_t GenerateObjectID() { return ++s_GlobalID; }

	protected:
		std::string m_ObjectName;
		uint32_t m_ObjectID;
	};
}
