//#include <iostream>
#include "Editor.h"
#include "ImGuiManager.h"
#include "EditorLayer.h"
//#include "RenderLayer.h"

namespace meme
{
	//Editor::Editor(const std::string& name) : Application(name)
	//{
	//	ImGuiManager::Init();
	//	auto edl = std::make_shared<EditorLayer>();
	//	m_Layers.emplace_back(edl);
	//	m_Layers.back()->OnAttach();
	//
	//	//m_Layers.emplace_back(std::make_unique<RenderLayer>(edl));
	//	//m_Layers.back()->OnAttach();
	//
	//}


	Editor::Editor(const std::string& name): Application(name)
	{
		ImGuiManager::Init();
		auto edl = std::make_shared<EditorLayer>(this);
		m_Layers.emplace_back(edl);
		m_Layers.back()->OnAttach();

		m_Layers.emplace_back(std::make_unique<RenderLayer>(edl));
		m_Layers.back()->OnAttach();
	}

	void Editor::Run()
	{
		fmt::print("running application!\n");

		m_IsRunning = true;
		GLFWwindow* window = Renderer::GetWindow();

		while (m_IsRunning && !glfwWindowShouldClose(window))
		{
			//Renderer::ClearColor(1, 0.0f, 0.0f, 1.0f);
			//Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Renderer::EnableDepth();
			//Renderer::SetDepthFunc(GL_LEQUAL);
			//Renderer::SetCullingMode(GL_BACK);
			//Renderer::SetAlphaBlending(false);

			//TODO: add sceneLayer, renderingLayer, etc.
			for (auto layer : m_Layers)
				layer->OnUpdate(0.16f);	//todo: calc frametime

			ImGuiManager::Prepare();

			for (auto layer : m_Layers)
				layer->OnImGuiRender();

			ImGuiManager::End();
			Renderer::SwapBuffers();
		}

		fmt::print("Stopping Application!");
		ImGuiManager::ShutDown();
		Renderer::ShutDown();
	}

	void Editor::StopRunning()
	{
		m_IsRunning = false;
	}

	Application* CreateApplication()
	{
		return new Editor("Editor");
	}
}