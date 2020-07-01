#include "precomp.h"
#include "PostProcessing.h"
#include "Renderer.h"
#include "ScreenQuad.h"
#include "ShaderManager.h" 
#include "Light/LightManager.h"


void PostProcessing::ShadowCastGLSL(Camera& camera, Gbuffer& gBuffer)
{
	Renderer::DisableDepth();
	Shader& shader = ShaderManager::GetShader("shadow_cast");

	static bool castShadows = false;
	static bool useZbuffer = false;
	static float epsilon = 0.00001f;
	ImGui::Checkbox("Cast Shadows", &castShadows);
	ImGui::SliderFloat("epsilon", &epsilon, 0, 0.1f, "%.5f");

	if(castShadows)	ImGui::Checkbox("Use Z-buffer", &useZbuffer);
	
	const float ambient = LightManager::GetAmbientLight();

	shader.Bind();
	//shader.SetInt("gPosition",0);
	//shader.SetInt("gNormal",1);
	//shader.SetInt("gAlbedoSpec",2);
	//shader.SetInt("zBuffer",3);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetPositionID());
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetNormalID());
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetAlbedoSpecID());
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetZBufferTexID());


	const glm::vec3& lightDir = LightManager::GetDirectionalLight();
	shader.Bind();
	shader.SetVec3f("u_cameraPos", camera.GetPosition());
	shader.SetFloat("u_screenWidth", static_cast<float>(SCREENWIDTH));
	shader.SetFloat("u_screenHeight", static_cast<float>(SCREENHEIGHT));
	shader.SetUniformMat4f("u_inv_projMatrix", glm::inverse(camera.GetProjectionMatrix()));
	shader.SetUniformMat4f("u_inv_viewMatrix", glm::inverse(camera.GetViewMatrix()));
	 
	shader.SetFloat("u_near_plane", camera.GetNearPlaneDist());
	shader.SetFloat("u_far_plane", camera.GetFarPlaneDist());
	shader.SetFloat("u_ambientLight", ambient);
	shader.SetFloat("u_epsilon", epsilon);
	 
	 
	shader.SetVec3f("u_lightDir", lightDir);
	shader.SetInt("u_shadowCast", castShadows);
	shader.SetInt("u_useZbuffer", useZbuffer);

	Renderer::DrawScreenQuad();
}
