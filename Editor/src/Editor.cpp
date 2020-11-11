#include "Editor.h"
#include <iostream>

namespace meme
{
	void Editor::Run()
	{
		std::cout << "running application!\n";
	}
	
	Application* CreateApplication()
	{
		return new Editor("Editor");
	}
}