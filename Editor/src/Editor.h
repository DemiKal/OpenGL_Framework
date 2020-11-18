#pragma once

#include "Core/Application.h"
#include "EditorLayer.h"
#include "ImGuiManager.h"

namespace meme
{
	class Editor : public Application
	{
	public:
		Editor(const std::string& name) : Application(name)
		{
			ImGuiManager::Init();
			m_Layers.emplace_back(new EditorLayer());
			m_Layers.back()->OnAttach();
		}

		void Run() override;
	};
}