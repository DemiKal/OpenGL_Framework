#include "RenderLayer.h"

#include "GameObject/Components/EntityComponents.h"
#include "Rendering/ShaderManager.h"
#include "Editor.h"



RenderLayer::RenderLayer(std::shared_ptr<EditorLayer> edl)
	:
	Layer("RenderLayer"),
	m_EditorLayer(std::move(edl))
{

}

void RenderLayer::OnAttach()
{
	FrameBufferSpecs specs =
	{
		specs.Name = "Camera Framebuffer",
		specs.Width = 1920,
		specs.Height = 1080,
	};

	auto* ed = m_Editor;	//this needs to be set 
	CommandBuffer& cb = ed->GetCommandBuffer();
	m_FramebufferCamera = &cb.GenerateFrameBuffer(specs);

}

void RenderLayer::OnDetach()
{
}


void RenderLayer::RenderCamera() const
{
	Renderer& renderer = m_Editor->GetRenderer();

	auto view2 = m_EditorLayer->m_Registry.view<CameraComponent>();
	auto meshes = m_EditorLayer->m_Registry.view<MeshComponent, TransformComponent>();

	m_FramebufferCamera->Bind();

	auto [fbWidth, fbHeight] = m_FramebufferCamera->GetSize();
	const auto prevViewport = renderer.GetViewPort();
	renderer.SetViewPort(0, 0, fbWidth, fbHeight);

	for (auto entity : view2)
	{
		renderer.SetClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		renderer.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.EnableDepth();

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

	m_FramebufferCamera->Unbind();
	renderer.SetViewPort(prevViewport);

}

void RenderLayer::OnUpdate(float dt)
{
	RenderCamera();

	ComputeShader& compute = ShaderManager::GetComputeShader("raytrace");
	compute.Bind();
	const Texture2D& computeTexture = compute.GetComputeTexture();
	const uint32_t texW = computeTexture.GetWidth();
	const uint32_t texH = computeTexture.GetHeight();
	computeTexture.Bind();


	glDispatchCompute(static_cast<GLuint>(texW), static_cast<GLuint>(texH), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	compute.Unbind();
	compute.GetComputeTexture().Unbind();
}

void RenderLayer::OnImGuiRender(float dt)
{
	ComputeShader& compute = ShaderManager::GetComputeShader("raytrace");

	ImGui::Begin("ray trace test");
	const auto size = ImGui::GetContentRegionAvail();
	auto* texPtr = reinterpret_cast<void*>(static_cast<intptr_t>(compute.GetComputeTexture().GetID()));
	ImGui::ImageButton(texPtr, size, ImVec2(0, 1), ImVec2(1, 0), 0);

	ImGui::End();
}