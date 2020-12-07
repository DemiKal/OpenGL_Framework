#pragma once
//#include "precomp.h"
#include "Core/Layer.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/MeshManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h"

int main(int argc, char** argv);

namespace meme
{
	class Application
	{
	public:
		virtual ~Application() = default;

		explicit Application(std::string name) : m_Name(std::move(name))
		{
			EntityManager::Init();
			m_Renderer.Init();
			ShaderManager::Init();
			MeshManager::Init();
			//ImGuiManager::Init(); to editor
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