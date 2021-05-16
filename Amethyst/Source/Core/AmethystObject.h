#pragma once
#include <memory>
#include <string>

namespace Amethyst
{
	class Context;
	
	//Global
	static uint32_t g_ObjectID;

	class AmethystObject
	{
	public:
		AmethystObject(Context* context = nullptr)
		{
			m_Context = context;
			m_ID = GenerateObjectID();
		}

		//Name
		const std::string& RetrieveObjectName() const { return m_Name; }

		//ID
		const uint32_t RetrieveObjectID() const { return m_ID; }
		void SetObjectID(const uint32_t objectID) { m_ID = objectID; }
		static uint32_t GenerateObjectID() { return ++g_ObjectID; }

		//CPU and GPU
		const uint64_t RetrieveCPUSize() const { return m_Size_CPU; }
		const uint64_t RetrieveGPUSize() const { return m_Size_GPU; }

	protected:
		//Execution Context
		Context* m_Context = nullptr;

		std::string m_Name;
		uint32_t m_ID = 0;

		uint64_t m_Size_GPU;
		uint64_t m_Size_CPU;
	};
}
