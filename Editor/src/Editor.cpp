#include <iostream>
#include "Editor.h"
#include "ImGuiManager.h"

namespace meme
{
	void Editor::Run()
	{
		std::cout << "running application!\n";

		static bool yahoo = true;
		GLFWwindow* window = Renderer::GetWindow();

		while (!glfwWindowShouldClose(window))
		{
			Renderer::ClearColor(1, 0.0f, 0.5f, 1.0f);
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Renderer::EnableDepth();
			//Renderer::SetDepthFunc(GL_LEQUAL);
			//Renderer::SetCullingMode(GL_BACK);
			//Renderer::SetAlphaBlending(false);
			
			//TODO: add sceneLayer, renderingLayer, etc.
			for (auto  layer : m_Layers)
			{
				layer->OnUpdate(0.16f);	//todo: calc frametime
			}

			ImGuiManager::Prepare();

			for (auto  layer : m_Layers)
			{
				layer->OnImGuiRender();
			}

			ImGuiManager::End();
			Renderer::SwapBuffers();
		}

		ImGuiManager::ShutDown();
		Renderer::ShutDown();
	}

	Application* CreateApplication()
	{
		return new Editor("Editor");
	}
}