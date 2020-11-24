#pragma once

#include "Core/Application.h"
#include "EditorLayer.h"

namespace meme
{
	class Editor final : public Application
	{
		bool m_IsRunning = false;

	public:
		explicit Editor(const std::string& name);

		void Run() override;
		void StopRunning();

		template<typename T>
		[[nodiscard]] T* GetLayer()
		{
			for (auto layer : m_Layers)
			{
				if (dynamic_cast<T*>(layer.get()))
					return  dynamic_cast<T*>(layer.get());
			}

			return nullptr;
		}
	};
}