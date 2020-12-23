#include "precomp.h"
#include "Gbuffer.h"

#include "FrameBuffer.h"
#include "GameObject/Components/Texture2D.h"
#include "Light/LightManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/ShaderManager.h"

Gbuffer::Gbuffer(const unsigned int width, const unsigned int height)
	:
	m_bufferID(0),
	m_gPosition(0),
	m_gNormal(0),
	m_gAlbedoSpec(0),
	m_RBO(0),				//depth
	m_lightingShader(0)
{
	glGenFramebuffers(1, &m_bufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);

	glGenTextures(1, &m_gPosition);
	glBindTexture(GL_TEXTURE_2D, m_gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);
	// normal color buffer
	glGenTextures(1, &m_gNormal);
	glBindTexture(GL_TEXTURE_2D, m_gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &m_gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
	// finally check if framebuffer is complete
	while (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{


		fmt::print("Framebuffer not complete!\n");
	}

	const Texture2D zBufferTexPosition(
		GL_DEPTH_COMPONENT24,
		width,
		height,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT);

	m_zBufferTexID = zBufferTexPosition.GetID();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBufferTexPosition.GetID(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t Gbuffer::GetZBufferTexID() const
{
	return m_zBufferTexID;
}

void Gbuffer::SetShader(const std::string& shaderName)
{
	m_lightingShader = ShaderManager::GetShaderIdx(shaderName);
	 //m_lightingShader = ShaderManager::GetShader(shaderName).GetID();
}

unsigned int Gbuffer::GetID() const
{
	return m_bufferID;
}
unsigned int Gbuffer::GetPositionID() const { return m_gPosition; }
unsigned int Gbuffer::GetNormalID() const { return m_gNormal; }
unsigned int Gbuffer::GetAlbedoSpecID() const { return m_gAlbedoSpec; }
unsigned Gbuffer::GetRBO_ID() const { return m_RBO; }

inline void Gbuffer::Enable()
{

}

void Gbuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
}

void Gbuffer::BindShader() const
{
	ShaderManager::GetShader(m_lightingShader).Bind();
}

void Gbuffer::LightingPass(const FrameBuffer& frameBuffer, Camera& lightCam, const GLuint zbufferTex) const
{

	frameBuffer.Bind();
	//Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //TODO: REMOVE RENDERER SINGLETON!

	Shader& shader = ShaderManager::GetShader(m_lightingShader);
	shader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GetPositionID());
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, GetNormalID());
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, GetAlbedoSpecID());

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, zbufferTex);

	
	glm::vec3  lightDir = LightManager::GetDirectionalLight();
	float ambientLight = LightManager::GetAmbientLight();
	shader.SetVec3f("u_globalLightDir", lightDir);
	shader.SetVec3f("u_lightPos", lightCam.GetPosition());
	shader.SetFloat("u_ambientLight", ambientLight);
	shader.SetUniformMat4f("u_lightMatrix", lightCam.GetViewProjectionMatrix());
	static bool drawShadows = true;
	//ImGui::Checkbox("shadow", &drawShadows);
	shader.SetInt("u_useShadows", drawShadows);
	Renderer::DrawScreenQuad();

}