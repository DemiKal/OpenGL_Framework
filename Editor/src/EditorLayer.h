#pragma once
#include "Core/Layer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/Buffer/FrameBuffer.h"
#include "Gizmos/Gizmo.h"

namespace meme {
	class Editor;
}
class Gizmo;

class EditorLayer : public Layer
{

public:
	meme::Editor* m_Editor;
	entt::registry m_Registry;
	entt::entity m_Selected{ entt::null };
	Camera m_EditorCamera;
	//FrameBuffer* m_SceneFrame;
	ImVec2 m_ImGuiRegionSize;
	ImGuizmo::OPERATION m_TransformWidgetOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE m_TransformWidgetMode = ImGuizmo::MODE::LOCAL;
	std::vector<Gizmo*> m_Gizmos;
	Texture2D m_Skybox;

	EditorLayer(meme::Editor* editor);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
	void DrawDebugVisuals(float dt);
	void RenderSkybox();
	void RenderScene(float dt);
	void DrawGizmos(float dt);
	void DrawCameraInspector(float dt);
	void OnInput(float dt);
	void EnableDockSpace();
	//void DrawMenuBar();
	Camera& GetEditorCamera();

	void DrawGizmoMenu();




	void RenderViewportPanel(float dt);
	void RenderInspectorPanel(float dt);
	void RenderSceneHierarchyPanel(float dt);


	void DrawUIComponent(TransformComponent& t);
	void DrawUIComponent(MeshComponent& mc, float dt);
	void DrawUIComponent(CameraComponent& cc, entt::entity entity, float dt);
	void DrawUIComponent(const glm::mat4& m, const std::string& label, float dt);


	void DrawVec3Component(const std::string& label, glm::vec3& vec, float resetVal);

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
};

