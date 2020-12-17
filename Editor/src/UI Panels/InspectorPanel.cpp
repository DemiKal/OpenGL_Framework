#include "UI Panels/UI includes.h"
class EditorLayer;
 
void DrawUIComponent(CameraComponent& cc, entt::registry& registry, entt::entity entity, const float dt);
void DrawUIComponent(TransformComponent& t);
void DrawUIComponent(MeshComponent& mc, const float dt);
void DrawUIComponent(BVHComponent& mc, EditorLayer* edl, const float dt);

template <typename T, typename uiFunc>
void DrawUIComponent(const std::string& label, entt::registry& registry, entt::entity entity, uiFunc func)
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	if (registry.has<T>(entity))
	{
		//ImGui::PushID(label.c_str());

		T& component = registry.get<T>(entity);//reinterpret_cast<void*>(typeid(T).hash_code())
		//const bool open = ImGui::CollapsingHeader(label.c_str(), treeNodeFlags);
		const ImVec2 avail = ImGui::GetContentRegionAvail();
		const bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), treeNodeFlags, label.c_str());
		//ImGui::GetCurrentContext()
		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

		ImGui::SameLine(avail.x - lineHeight * 0.5f);
		if (ImGui::Button(" ... ", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;

		if (ImGui::BeginPopup("ComponentSettings"))
		{
			auto isTransf = std::is_same<T, TransformComponent>::value;

			//ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TextDisabled)

			if (isTransf) ImGui::TextDisabled("Remove component");
			else if (ImGui::MenuItem("Remove component"))
			{
				removeComponent = true;
			}

			ImGui::MenuItem("Reset component");	//TODO: add these extras
			ImGui::MenuItem("Copy component");	//TODO: add these extras

			ImGui::EndPopup();
		}

		if (open)
		{
			func(component);
			ImGui::TreePop();
		}

		if (removeComponent)
		{
			registry.remove<T>(entity);
		}
	}
}



