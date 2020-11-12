#pragma once

#include "../src/Core/Application.h"
#include "EditorLayer.h"

namespace meme
{

	class Editor : public Application
	{
	public:
		Editor(const std::string& name) : Application(name)
		{
			m_Layers.emplace_back( new EditorLayer() );

		}

		void Run();
		
	};


	
}