#pragma once
#include <fmt/format.h>
int main(int argc, char** argv);

namespace meme {
	class Application
	{
	public:
		void Run() 
		{ 
			fmt::print("yikes lel\n");
		}
	private:
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}