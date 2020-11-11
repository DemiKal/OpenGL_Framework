#pragma once
#include "precomp.h"
#include <fmt/format.h>
#include "Core/Layer.h"
#include "GameObject/EntityManager.h"
#include "Rendering/Renderer.h"
//#include "Rendering/ShaderManager.cpp" 


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
			//ShaderManager::Init();
		}

		void Run() 
		{ 
			
		}
		
		//std::vector<Layer*>& GetLayers()
		
	private:
		friend int ::main(int argc, char** argv);
		std::vector<Layer*> m_Layers;

	protected:
		std::string m_Name;
	};

	Application* CreateApplication();
}