#include "RenderLayer.h"
//#include "EditorLayer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
//el->m_Registry#include "GameObject/Components/Mesh.h"

RenderLayer::RenderLayer(std::shared_ptr<EditorLayer> edl) 
	:  Layer("RenderLayer"), 
		m_Editor(edl.get())
{

}

void RenderLayer::OnAttach()
{
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
	static const auto originalCamPos = glm::vec3(0, 3, 16);
	static Camera camera(originalCamPos, 70, float(SCREENWIDTH) / float(SCREENHEIGHT), 0.1f, 700.0f);
	Shader& shader = ShaderManager::GetShader("basic");

	//auto& reg = ;
	auto view = m_Editor->m_Registry.view<MeshComponent, TransformComponent>();
	static float f = 0;

	for (auto entity : view)
	{
		auto [meshC, transf] = m_Editor->m_Registry.get<MeshComponent, TransformComponent>(entity);
		
		uint32_t idx = meshC.MeshIdx;
		Mesh& mesh = Mesh::m_Meshes[idx];
		//
		mesh.Draw(camera, transf.Transform, shader);
	}
}

void RenderLayer::OnImGuiRender()
{
}
