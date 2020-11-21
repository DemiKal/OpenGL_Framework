#pragma once

#include "Core/Application.h"
#include "EditorLayer.h"
#include "renderLayer.h"
#include "ImGuiManager.h"

namespace meme
{
	class Editor : public Application
	{
		bool m_IsRunning = false;
		
	public:
		explicit Editor(const std::string& name);

		void Run() override;
		void StopRunning();
	};
}