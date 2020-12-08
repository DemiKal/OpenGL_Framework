//#include "InspectorPanel.h"
////#include //"
//#include "ImGuiManager.h"
//#include "GameObject/Components/EntityComponents.h"
#include "EditorLayer.h"

void  EditorLayer::RenderInspectorPanel(float dt)
{
	ImGui::Begin("Inspector");
	if (m_Selected != entt::null)
	{
		auto& tfc = m_Registry.get<TransformComponent>(m_Selected);
		DrawUIComponent(tfc, "Transform Component");
		if (m_Registry.has<MeshComponent>(m_Selected))
		{
			auto& mc = m_Registry.get<MeshComponent>(m_Selected);
			DrawUIComponent(mc, "Mesh Component");
		}
		if (m_Registry.has<CameraComponent>(m_Selected))
		{
			auto& cc = m_Registry.get<CameraComponent>(m_Selected);
			DrawUIComponent(cc, "Camera Component", m_Selected);
		}
	}

	ImGui::End();

}