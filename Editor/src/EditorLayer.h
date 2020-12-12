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
	FrameBuffer m_SceneFrame;
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
	void RenderScene(const float dt);
	void DrawGizmos(const float dt);
	void DrawCameraInspector(float dt);
	void OnInput(const float dt);
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
			T& component = registry.get<T>(entity);//reinterpret_cast<void*>(typeid(T).hash_code())
			const bool open = ImGui::CollapsingHeader(label.c_str(), treeNodeFlags);
			
			if (open)
			{
				func(component);
			//	ImGui::TreePop();
			}
		}
	}
};

