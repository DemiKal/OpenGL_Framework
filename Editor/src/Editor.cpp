#include <iostream>
#include "Editor.h"
#include "ImGuiManager.h"

namespace meme
{
	void Editor::Run()
	{
		std::cout << "running application!\n";

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
			//
			
			ImGui::ShowDemoWindow(&yahoo);
			//

			ImGuiManager::End();
			Renderer::SwapBuffers(window);
		}

		ImGuiManager::ShutDown();
		Renderer::ShutDown();

	}

	Application* CreateApplication()
	{
		return new Editor("Editor");
	}
}