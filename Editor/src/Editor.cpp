#include "Editor.h"
#include "DebugRenderLayer.h"
#include "EditorLayer.h"
#include "RenderLayer.h"
#include "Rendering/Renderer.h"

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
		auto edl = std::make_shared<EditorLayer>(this);
		m_Layers.emplace_back(edl);
		m_Layers.emplace_back(std::make_unique<RenderLayer>(edl));
		m_Layers.emplace_back(std::make_shared<DebugRenderLayer>(edl));


		for (const auto& layer : m_Layers)
			layer->m_Editor = this;

		for (const auto& layer : m_Layers) 
			layer->OnAttach();
	}


	void Editor::Run()
	{
		fmt::print("running application!\n");

		m_IsRunning = true;
		GLFWwindow* window = m_Renderer.GetWindow();

		m_Renderer.EnableDepth();
		m_Renderer.SetVSync(true);

		float prevFrameTime = static_cast<float>(glfwGetTime());
		float currentFrameTime = prevFrameTime;

		

		while (m_IsRunning && !glfwWindowShouldClose(window))
		{
			//Renderer::SetClearColor(0, 1.0f, 0.0f, 1.0f);
			//Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//Renderer::SetDepthFunc(GL_LEQUAL);
			//Renderer::SetCullingMode(GL_BACK);
			//Renderer::SetAlphaBlending(false);
			prevFrameTime = currentFrameTime;
			currentFrameTime = static_cast<float>(glfwGetTime());
			float dt = (currentFrameTime - prevFrameTime);
			//fmt::print("fps: {}\n", 1.0f / dt);
			//TODO: add sceneLayer, renderingLayer, etc.
			for (const auto& layer : m_Layers)
				layer->OnUpdate(dt);	//todo: calc frametime

			ImGuiManager::Prepare();
			//glDisable(GL_DEBUG_OUTPUT);
			for (const auto& layer : m_Layers)
				layer->OnImGuiRender(dt);
	//		glEnable(GL_DEBUG_OUTPUT);
			ImGuiManager::End();

			m_Renderer.SwapBuffers();
		}

		fmt::print("Stopping Application!\n");
		ImGuiManager::ShutDown();
		m_Renderer.ShutDown();
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