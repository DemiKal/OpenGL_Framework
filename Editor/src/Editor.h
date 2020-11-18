#pragma once

#include "Core/Application.h"
#include "EditorLayer.h"
#include "renderLayer.h"
#include "ImGuiManager.h"

namespace meme
{
	class Editor : public Application
	{
	public:
		Editor(const std::string& name) : Application(name)
		{
			ImGuiManager::Init();
			auto edl = std::make_shared<EditorLayer>();
			m_Layers.emplace_back(edl);
			m_Layers.back()->OnAttach();

			m_Layers.emplace_back(std::make_unique<RenderLayer>(edl));
			m_Layers.back()->OnAttach();
		}

		void Run() override;
	};
}