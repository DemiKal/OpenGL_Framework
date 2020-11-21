#include "RenderLayer.h"
//#include "EditorLayer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
#include <ImGuiManager.h>
#include <Rendering/Renderer.h>

RenderLayer::RenderLayer(std::shared_ptr<EditorLayer> edl)
	: Layer("RenderLayer"),
	m_EditorLayer(edl.get())
{
	fb = new FrameBuffer(1920, 1080);

}

void RenderLayer::OnAttach()
{
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
	 
	//static Camera camera(glm::vec3(0, 3, 16), 70, float(SCREENWIDTH) / float(SCREENHEIGHT), 0.1f, 700.0f);
	Shader& shader = ShaderManager::GetShader("basic");

	//auto& reg = ;
	auto view = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();
	static float f = 0;


	//fb.Bind();
	//Renderer::ClearColor(1, 0.0f, 0.5f, 1.0f);
	//Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Camera& camera = m_EditorLayer->GetEditorCamera();
	fb->Bind();
	Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderer::ClearColor(1.0F, 1.0f, 1.0f, 1.0f);
	Renderer::EnableDepth();
	for (auto entity : view)
	{
		auto [meshC, transf] = m_EditorLayer->m_Registry.get<MeshComponent, TransformComponent>(entity);

		uint32_t idx = meshC.MeshIdx;
		Mesh& mesh = Mesh::m_Meshes[idx];
		
		

		mesh.Draw(camera, transf.CalcMatrix(), shader);
	}

	fb->Unbind();
}

void RenderLayer::OnImGuiRender()
{
	const auto texId = fb->GetTexture().GetID();

	ImGui::Begin("Scene");
	const ImVec2 avail_size = ImGui::GetContentRegionAvail();
	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)),
		avail_size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

