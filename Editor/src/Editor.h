#pragma once

#include "../src/Core/Application.h"
namespace meme
{

	class Editor : public Application
	{
	public:
		Editor(const std::string& name) : Application(name)
		{

		}

		void Run();
		
	};


	
}