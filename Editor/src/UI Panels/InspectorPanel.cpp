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
		DrawUIComponent<TransformComponent>("Transform", m_Registry, m_Selected,
			[=](auto& t) {	DrawUIComponent(t);	 });

		DrawUIComponent<MeshComponent>("Mesh Component", m_Registry, m_Selected,
			[=](auto& t) {	DrawUIComponent(t, dt);	 });

		DrawUIComponent<CameraComponent>("Camera Component", m_Registry, m_Selected,
			[=](CameraComponent& t) {	DrawUIComponent(t, m_Selected, dt );	 });
	}

	ImGui::End();

}
