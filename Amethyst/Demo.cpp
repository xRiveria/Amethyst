#include "Source/Editor/Editor.h"
#include <Windows.h>

int main(int argc, int argv[])
{
	Amethyst::Editor g_Editor;
	g_Editor.OnUpdate();

	return 0;
}