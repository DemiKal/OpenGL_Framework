#pragma once
#include "precomp.h"
#include <fmt/format.h>
#include "Core/Layer.h"
#include "GameObject/EntityManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h"
//#include "Rendering/ShaderManager.cpp" 
//#include "Geometry/ShaderManager.cpp" 


int main(int argc, char** argv);

namespace meme
{
	class Application
	{
	public:
		Application(const std::string& name) : m_Name(name)
		{
			EntityManager::Init();
			Renderer::Init();
			ShaderManager::Init();
		}

		void Run()
		{		
			glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);

			GLFWwindow* window = Renderer::m_Window;
			while (!glfwWindowShouldClose(window))
			{
				Renderer::ClearColor(1, 0, 1, 1);
				Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				Renderer::SwapBuffers(window);
			}

			glfwDestroyWindow(window);

			glfwTerminate();
		}
		 
	private:
		friend int ::main(int argc, char** argv);
		std::vector<Layer*> m_Layers;

	protected:
		std::string m_Name;
	};

	Application* CreateApplication();
}