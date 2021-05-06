#include "Amethyst.h"
#include "AmethystObject.h"

namespace Amethyst
{
	uint32_t g_ObjectID;

	AmethystObject::AmethystObject()
	{
		m_ID = GenerateObjectID();
	}
}