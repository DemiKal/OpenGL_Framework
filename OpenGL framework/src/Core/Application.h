#pragma once
//#include "precomp.h"
#include "Core/Layer.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/MeshManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/ImGuiManager.h"

int main(int argc, char** argv);
//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
//glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE

const std::vector<std::tuple<int, int>> windowHints =
{
	{GLFW_CONTEXT_VERSION_MAJOR,	4},
	{GLFW_CONTEXT_VERSION_MINOR,	5},
	{GLFW_OPENGL_PROFILE,			GLFW_OPENGL_COMPAT_PROFILE},
	{GLFW_OPENGL_FORWARD_COMPAT,	GL_TRUE },
	{GLFW_MAXIMIZED,				GL_TRUE},
	{GLFW_DECORATED,				GL_TRUE}
	//{GLFW_SCALE_TO_MONITOR ,		GL_FALSE}
};

namespace meme
{
	class Application
	{
	public:
		virtual ~Application() = default;

		explicit Application(std::string name) : m_Name(std::move(name))
		{
			EntityManager::Init();
			m_Renderer.Init(windowHints);
			ShaderManager::Init("../OpenGL Framework/shaders");
			MeshManager::Init();
			ImGuiManager::Init(m_Renderer);
		}

		virtual void Run() {}
		Renderer& GetRenderer() { return m_Renderer; }

	private:
		friend int ::main(int argc, char** argv);

	protected:
		std::vector<std::shared_ptr<Layer>> m_Layers;
		std::string m_Name;
		Renderer m_Renderer;
	};

	Application* CreateApplication();
}