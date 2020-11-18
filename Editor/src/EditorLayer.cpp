#include "EditorLayer.h"
#include "ImGuiManager.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Camera.h"

void EditorLayer::OnAttach()
{
	//EntityManager em;
	//em.GetEntities();
	EntityManager::LoadModel("res/meshes/spyro/spyro.obj", "spyro"); //todo fix addenetity!
	fmt::print("on attach!");
}

void EditorLayer::OnDetach()
{

}

void EditorLayer::OnUpdate(float dt)
{
	static const auto originalCamPos = glm::vec3(0, 3, 16);
	static Camera camera(originalCamPos, 70, float(SCREENWIDTH)/float(SCREENHEIGHT), 0.1f, 700.0f);
	//static ShaderManager::;

	auto& entities = EntityManager::GetEntities();

	for (auto entity : entities)
	{
		entity->SetShader("basic");
		entity->m_modelMatrix = glm::mat4(1.0f);
		entity->Draw(camera);
	}
}

void EditorLayer::OnImGuiRender()
{
	static bool yahoo = true;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::ShowDemoWindow(&yahoo);

	ImGui::Begin("Debug");
	ImGui::Text("Ayy lmao");
	ImGui::End();
}