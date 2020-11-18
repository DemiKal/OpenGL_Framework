#include "EditorLayer.h"
#include "ImGuiManager.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"

void EditorLayer::OnAttach()
{
	//EntityManager em;
	//em.GetEntities();
	//EntityManager::LoadModel("res/meshes/spyro/spyro.obj", "spyro"); //todo fix addenetity!
	fmt::print("on attach!");

	auto e = m_Registry.create();
	m_Registry.emplace<TransformComponent>(e);

	auto& meshComp = m_Registry.emplace<MeshComponent>(e);

	auto ret = Mesh::LoadFromFile("res/meshes/spyro/spyro.obj", aiPostProcessSteps::aiProcess_Triangulate);

	if (ret)
	{
		meshComp.initialized = true;
		meshComp.meshIdx = ret.value();
	}


	//mkonjibuvgyv
	//m_Registry.get<MeshCOmponent
	//m_Registry.emplace<>();
	//m_Registry.emplace<Texture2DComponent>(e);
	//....m_Registry.emplace<MeshComponent>(e);
	//m_Registry.emplace<> 

}

void EditorLayer::OnDetach()
{

}

void EditorLayer::OnUpdate(float dt)
{
	static const auto originalCamPos = glm::vec3(0, 3, 16);
	static Camera camera(originalCamPos, 70, float(SCREENWIDTH) / float(SCREENHEIGHT), 0.1f, 700.0f);
	Shader& shader = ShaderManager::GetShader("basic");


	auto view = m_Registry.view<MeshComponent, TransformComponent>();

	for (auto entity : view)
	{
		auto [meshC, transf] = m_Registry.get< MeshComponent, TransformComponent>(entity);
		uint32_t idx = meshC.meshIdx;
		Mesh& mesh = Mesh::m_Meshes[idx];
		
		mesh.Draw(camera, transf.Transform, shader);
	}

	// auto& entities = EntityManager::GetEntities();
	//
	//for (auto entity : entities)
	//{
	//	entity->SetShader("basic");
	//	entity->Draw(camera);
	//}
}

void EditorLayer::OnImGuiRender()
{
	static bool yahoo = true;
	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::ShowDemoWindow(&yahoo);


	ImGui::Begin("Debug");
	ImGui::Text("Ayy lmao");
	ImGui::End();
}