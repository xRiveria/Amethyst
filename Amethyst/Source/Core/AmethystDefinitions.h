#pragma once

//Version
#define Amethyst_Version "0.1 WIP"

//Platform
#define API_GRAPHICS_VULKAN
//#define API_GRAPHICS_D3D12
//#define API_GRAPHICS_OPENGL

//Debug
#define AMETHYST_DEBUG_BREAK() __debugbreak()

//Assert
#ifdef DEBUG
#define AMETHYST_ASSERT(expression) assert(expression)
#else
#define AMETHYST_ASSERT(expression) \
if (!(##expression))				\
{									\
	AMETHYST_ERROR(#expression);	\
	AMETHYST_DEBUG_BREAK();			\
}					
#endif

//Delete
#define AMETHYST_DELETE(x)	 \
if (x)						 \
{							 \
	delete x;				 \
	x = nullptr;			 \
}