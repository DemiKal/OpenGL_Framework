#include "SceneHierarchyPanel.h"
#include "ImGuiManager.h"
//#include "GameObject/Components/EntityComponents.h"
#include "EditorLayer.h"

void EditorLayer::RenderSceneHierarchyPanel(float dt)
{
	const auto flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Entities", NULL, flags);
	auto view = m_Registry.view<TagComponent>();
	bool otherPopup = false;
	for (auto entity : view)
	{
		auto tc = m_Registry.get<TagComponent>(entity);
		const auto nodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;

		//ImGui::Text("(You can also right-click me to open the same popup as above.)");
		//if (ImGui::TreeNode(name.Name,, m_Selected == entity))
		auto flags = m_Selected == entity ? ImGuiTreeNodeFlags_Selected : 0;
		//if (ImGui::TreeNodeEx(name.Name), flags)
		if (ImGui::Selectable(tc.Name, m_Selected == entity))
		{
			m_Selected = entity;

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete Entity"))
					//entityDeleted = true;{}
				{
				}
				ImGui::EndPopup();
			}
		}
	}

	auto isHovered = ImGui::IsWindowHovered();
	bool leftClick = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered;
	bool rightClick = ImGui::IsMouseClicked(ImGuiMouseButton_Right) && isHovered;

	if (leftClick)m_Selected = entt::null;
	if (rightClick && !otherPopup) ImGui::OpenPopup("ContextMenu");

	if (ImGui::BeginPopup("ContextMenu"))
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			int count = 0;
			for (auto entity : view)
			{
				auto tc = m_Registry.get<TagComponent>(entity);
				auto nm = tc.Name.data();
				std::string snm{ nm };
				auto find = snm.find("GameObject (");
				if (strcmp(snm.c_str(), "GameObject") == 0) count++;
				if (find != std::string::npos) count++;
			}
			auto newEntity = m_Registry.create();
			m_Registry.emplace<TransformComponent>(newEntity);

			std::string* newName = new std::string("GameObject" + (count > 0 ? " (" + std::to_string(count) + ")" : ""));
			auto zzz = entt::hashed_string{ newName->c_str() };
			auto tc = m_Registry.emplace<TagComponent>(newEntity, zzz);
		}
		ImGui::EndPopup();
	}
	ImGui::End();
}