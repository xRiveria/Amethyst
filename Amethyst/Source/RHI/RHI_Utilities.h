#pragma once
#include "GL/glew.h"
#include <string>

namespace Amethyst
{
	inline std::string RetrieveRHIVendor()
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	}

	inline std::string RetrieveRHIRenderer()
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	}

	inline std::string RetrieveRHIVersion()
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	}
}