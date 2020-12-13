#include "RenderLayer.h"

#include <utility>
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Renderer.h"
#include "Editor.h"

RenderLayer::RenderLayer(std::shared_ptr<EditorLayer> edl)
	:
	Layer("RenderLayer"),
	m_EditorLayer(std::move(edl))// /*std::make_shared<EditorLayer>(*edl ))
{

}

void RenderLayer::OnAttach()
{

}

void RenderLayer::OnDetach()
{
}


void RenderLayer::RenderCamera() const
{
	Renderer& renderer = m_Editor->GetRenderer();
	
	auto view2 = m_EditorLayer->m_Registry.view<CameraComponent>();
	auto meshes = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();
	
	m_FramebufferCamera.Bind();  
	
	auto [fbWidth, fbHeight] = m_FramebufferCamera.GetSize();
	const auto prevViewport = renderer.GetViewPort();
	renderer.SetViewPort(0, 0, fbWidth, fbHeight);

	for (auto entity : view2)
	{
		renderer.SetClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		renderer .Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer .EnableDepth();

		const auto& cc = m_EditorLayer->m_Registry.get<CameraComponent>(entity);

		for (auto otherEntity : meshes)	 
		{
			if (otherEntity != entity)
			{
				auto [mc, t] = m_EditorLayer->m_Registry.get <MeshComponent, TransformComponent>(otherEntity);
				Shader& otherShader = ShaderManager::GetShader(mc.ShaderIdx);
				Mesh& otherMesh = MeshManager::GetMesh(mc.MeshIdx);
				otherMesh.Draw(cc.camera, t.CalcMatrix(), otherShader);
			}
		}
	}

	m_FramebufferCamera.Unbind();
	renderer.SetViewPort(prevViewport);

}

void RenderLayer::OnUpdate(float dt)
{
	RenderCamera();

	Shader& compute = ShaderManager::GetShader("raytrace");
	compute.Bind();
	const uint32_t texW = compute.m_ComputeWidth;
	const uint32_t texH = compute.m_ComputeHeight;
	compute.GetComputeTexture().Bind();

	static bool a = false;
	static GLuint m;
	//if(!a)
	//{
	//	glGenTextures(1, &m);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texW, texH, 0, GL_RGBA, GL_FLOAT, nullptr);
	//	glBindImageTexture(0, m, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	a = true;
	//}
	//
	//
	//glBindTexture(GL_TEXTURE_2D, m);
	//
	//texID = m;

	glDispatchCompute(static_cast<GLuint>(texW), static_cast<GLuint>(texH), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	compute.Unbind();
	compute.GetComputeTexture().Unbind();
}	
 
void RenderLayer::OnImGuiRender(float dt)
{
	Shader& compute = ShaderManager::GetShader("raytrace");

	ImGui::Begin("ray trace test");
	const auto size = ImGui::GetContentRegionAvail();
	auto* texPtr = reinterpret_cast<void*>(static_cast<intptr_t>(compute.GetComputeTexture().GetID()));
	ImGui::ImageButton(texPtr, size, ImVec2(0, 1), ImVec2(1, 0), 0);

	ImGui::End();
}