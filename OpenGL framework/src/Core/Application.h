#pragma once
#include "precomp.h"
#include <fmt/format.h>
#include "Core/Layer.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/MeshManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h"
//#include "src/ImGuiManager.h"
//#include "Core/ImGuiManager.h"
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
			MeshManager::Init();
			//ImGuiManager::Init(); to editor
		}

		virtual void Run() {};
		 

	private:
		friend int ::main(int argc, char** argv);

	protected:
		std::vector<std::shared_ptr<Layer>> m_Layers;
		std::string m_Name;
	};

	Application* CreateApplication();
}