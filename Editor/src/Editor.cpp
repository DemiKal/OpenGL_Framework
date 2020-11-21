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


	Editor::Editor(const std::string& name) : Application(name)
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

		Renderer::EnableDepth();
		float prevFrameTime = glfwGetTime();
		float currentFrameTime = prevFrameTime;

		while (m_IsRunning && !glfwWindowShouldClose(window))
		{
			//Renderer::ClearColor(0, 1.0f, 0.0f, 1.0f);
			//Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//Renderer::SetDepthFunc(GL_LEQUAL);
			//Renderer::SetCullingMode(GL_BACK);
			//Renderer::SetAlphaBlending(false);
			prevFrameTime = currentFrameTime;
			currentFrameTime = glfwGetTime();
			float dt =  (currentFrameTime - prevFrameTime)  ;

			//TODO: add sceneLayer, renderingLayer, etc.
			for (const auto& layer : m_Layers)
				layer->OnUpdate(dt);	//todo: calc frametime

			ImGuiManager::Prepare();

			for (const auto& layer : m_Layers)
				layer->OnImGuiRender(dt);

			//ImGui::Text("MS: %f", dt);
			//ImGui::Text("fps: %f", 1000.0f / dt);

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