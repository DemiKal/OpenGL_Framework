#include "EditorLayer.h"
#include "ImGuiManager.h"
//#include "GameObject/EntityManager.h"
#include "GameObject/Components/EntityComponents.h"



void EditorLayer::OnAttach()
{
	//EntityManager em;
	//em.GetEntities();
	//EntityManager::LoadModel("res/meshes/spyro/spyro.obj", "spyro"); //todo fix addenetity!
	fmt::print("on attach!");

	auto e = m_Registry.create();
	m_Registry.emplace<TransformComponent>(e);

	auto& meshComp = m_Registry.emplace<MeshComponent>(e, "res/meshes/spyro/spyro.obj", aiPostProcessSteps::aiProcess_Triangulate);
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
	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::ShowDemoWindow(&yahoo);
}