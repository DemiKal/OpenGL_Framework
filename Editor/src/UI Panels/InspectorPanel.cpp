#include "InspectorPanel.h"
//#include //"
#include "ImGuiManager.h"
#include "GameObject/Components/EntityComponents.h"

InspectorPanel::InspectorPanel(EditorLayer* edl) : Panel(edl)
{

};

void  InspectorPanel::OnImGuiRender(float dt)
{
	ImGui::Begin("Inspector");
	if (m_Edl->m_Selected != entt::null)
	{
		auto& tfc = m_Edl->m_Registry.get<TransformComponent>(m_Edl->m_Selected);
		m_Edl->DrawUIComponent(tfc, "Transform Component");
		if (m_Edl->m_Registry.has<MeshComponent>(m_Edl->m_Selected))
		{
			auto& mc = m_Edl->m_Registry.get<MeshComponent>(m_Edl->m_Selected);
			m_Edl->DrawUIComponent(mc, "Mesh Component");
		}
		if (m_Edl->m_Registry.has<CameraComponent>(m_Edl->m_Selected))
		{
			auto& cc = m_Edl->m_Registry.get<CameraComponent>(m_Edl->m_Selected);
			m_Edl->DrawUIComponent(cc, "Camera Component", m_Edl->m_Selected);
		}
	}

	ImGui::End();

}