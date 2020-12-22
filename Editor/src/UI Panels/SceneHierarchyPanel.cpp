#include "Rendering/ImGuiManager.h"
#include "GameObject/Components/EntityComponents.h"
#include "EditorLayer.h"
#include "Geometry/BVH/BVH.h"

void AddBVHComponent(entt::registry& registry, entt::entity selected, EditorLayer* edl)
{
	bool hasBVH = registry.has<BVHComponent>(selected);
	if (hasBVH) return;

	MeshComponent& mc = registry.get<MeshComponent>(selected);
	Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);

	BVHComponent& bvhc = registry.emplace<BVHComponent>(selected);
	edl->m_UpperLvlBVH->AddBVH(registry, selected, mc);
	bvhc.BVHidx = edl->m_UpperLvlBVH->GetBVHCount() - 1;
	
	edl->m_UpperLvlBVH->UpdateTopBVH(registry);
	
	//bvh.BuildBVH(mesh.m_PositionVertices);
}

//careful, selected can be invalid entity handle!
void DrawNode(entt::registry& registry, entt::entity entity, entt::entity& selected, EditorLayer* edl)
{
	auto& tag = registry.get<TagComponent>(entity).Name;

	ImGuiTreeNodeFlags flags = selected == entity ? ImGuiTreeNodeFlags_Selected : 0;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

	auto* const DiosMioLaCreatura = reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity)));
	const bool opened = ImGui::TreeNodeEx(DiosMioLaCreatura, flags, tag);
	if (ImGui::IsItemClicked())
	{
		selected = entity;
	}

	bool deleteEntity = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Entity"))
			deleteEntity = true;
		if (ImGui::BeginMenu("Add Component"))
		{
			if (registry.has<MeshComponent>(entity))
				ImGui::TextDisabled("Add Mesh");
			else if (ImGui::MenuItem("Add Mesh")) fmt::print("Added mesh!");

			if (registry.has<MeshComponent>(entity) && !registry.has<BVH>(entity))
			{
				if (ImGui::MenuItem("Add BVH"))	AddBVHComponent(registry, entity, edl);
			}
			else ImGui::TextDisabled("Add BVH");

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	if (opened)
	{
		//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		//bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag);
		//if (opened)
		//	ImGui::TreePop();
		ImGui::TreePop();
	}

	if (deleteEntity)
	{
		registry.destroy(entity);
		if (selected == entity)
			selected = entt::null;
	}
}

void EditorLayer::RenderSceneHierarchyPanel(float dt)
{
	ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoCollapse);

	m_Registry.each([&](auto ent)
		{
			DrawNode(m_Registry, ent, m_Selected, this);
		});

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsItemHovered())
		m_Selected = entt::null;

	if (ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if (ImGui::MenuItem("Create Empty Entity"))
		{
			const auto newEntity = m_Registry.create();
			m_Registry.emplace<TagComponent>(newEntity, "New Entity");
			m_Registry.emplace<TransformComponent>(newEntity);
		}
		ImGui::EndPopup();
	}

	ImGui::End();

	//const auto flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	//ImGui::Begin("Entities", NULL, flags);
	//auto view = m_Registry.view<TagComponent>();
	//bool otherPopup = false;
	//for (auto entity : view)
	//{
	//	auto tc = m_Registry.get<TagComponent>(entity);
	//	const auto nodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
	//
	//	//ImGui::Text("(You can also right-click me to open the same popup as above.)");
	//	//if (ImGui::TreeNode(name.Name,, m_Selected == entity))
	//	auto flags = m_Selected == entity ? ImGuiTreeNodeFlags_Selected : 0;
	//	//if (ImGui::TreeNodeEx(name.Name), flags)
	//	if (ImGui::Selectable(tc.Name, m_Selected == entity), ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowItemOverlap)
	//	{
	//		m_Selected = entity;
	//
	//		if (ImGui::BeginPopupContextItem())
	//		{
	//			if (ImGui::MenuItem("Delete Entity"))
	//				//deleteEntity = true;{}
	//			{
	//			}
	//			ImGui::EndPopup();
	//		}
	//	}
	//}
	//
	//auto isHovered = ImGui::IsWindowHovered();
	//bool leftClick = ImGui::IsMouseClicked(ImGuiMouseButton_Left) && isHovered;
	//bool rightClick = ImGui::IsMouseClicked(ImGuiMouseButton_Right) && isHovered;
	//
	//if (leftClick)
	//	m_Selected = entt::null;
	////if (rightClick && !otherPopup) ImGui::OpenPopup("ContextMenu");
	//
	//if (ImGui::BeginPopupContextItem("ContextMenu", ImGuiPopupFlags_MouseButtonRight))
	//{
	//	if (ImGui::MenuItem("Create Empty"))
	//	{
	//		int count = 0;
	//		for (auto entity : view)
	//		{
	//			auto tc = m_Registry.get<TagComponent>(entity);
	//			auto nm = tc.Name.data();
	//			std::string snm{ nm };
	//			auto find = snm.find("GameObject (");
	//			if (strcmp(snm.c_str(), "GameObject") == 0) count++;
	//			if (find != std::string::npos) count++;
	//		}
	//		auto newEntity = m_Registry.create();
	//		m_Registry.emplace<TransformComponent>(newEntity);
	//
	//		std::string* newName = new std::string("GameObject" + (count > 0 ? " (" + std::to_string(count) + ")" : ""));
	//		auto zzz = entt::hashed_string{ newName->c_str() };
	//		auto tc = m_Registry.emplace<TagComponent>(newEntity, zzz);
	//	}
	//	ImGui::EndPopup();
	//}
	//ImGui::End();
}