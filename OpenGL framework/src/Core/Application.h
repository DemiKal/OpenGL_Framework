#pragma once
#include "precomp.h"
#include <fmt/format.h>
#include "Core/Layer.h"
#include "GameObject/EntityManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h"
#include "Core/ImGuiManager.h"
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
			ImGuiManager::Init();
		}

		void Run()
		{
			static bool yahoo = true;
			GLFWwindow* window = Renderer::m_Window;
			
			while (!glfwWindowShouldClose(window))
			{
				Renderer::ClearColor(1, 0, 1, 1);
				Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
				//TODO: add sceneLayer, renderingLayer, etc.
				for (Layer* layer : m_Layers)
				{
					layer->OnUpdate(0.16f);
				}

				ImGuiManager::Prepare();

				ImGui::ShowDemoWindow(&yahoo);
				
				ImGuiManager::End();
				Renderer::SwapBuffers(window);
			}
			
			ImGuiManager::ShutDown();
			Renderer::ShutDown();
			
		}

	private:
		friend int ::main(int argc, char** argv);

	protected:
		std::vector<Layer*> m_Layers;
		std::string m_Name;
	};

	Application* CreateApplication();
}