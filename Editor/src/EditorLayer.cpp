#include "EditorLayer.h"
#include "ImGuiManager.h"
#include <GameObject/EntityManager.h>

void EditorLayer::OnAttach()
{
	EntityManager em;
}

void EditorLayer::OnDetach()
{

}

void EditorLayer::OnUpdate(float dt)
{

}

void EditorLayer::OnImGuiRender()
{
	static bool yahoo = true;
	ImGui::ShowDemoWindow(&yahoo);
}
