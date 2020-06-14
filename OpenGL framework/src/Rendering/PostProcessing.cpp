#include "precomp.h"
#include "PostProcessing.h"
#include "ShaderManager.h" 
#include "Light/LightManager.h"


void PostProcessing::ShadowCastGLSL(Camera& camera, Gbuffer& gBuffer)
{
	Shader& shader = ShaderManager::GetShader("shadow_cast");

	shader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetPositionID());
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetNormalID());
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetAlbedoSpecID());


	const glm::vec3& lightDir = LightManager::GetDirectionalLight();
	shader.SetVec3f("u_lightDir", lightDir);
	shader.SetFloat("u_screenWidth", SCREENWIDTH);
	shader.SetFloat("u_screenWidth", SCREENHEIGHT);
	shader.SetFloat("u_nearPlane", camera.GetNearPlaneDist());
	shader.SetFloat("u_nearPlane", camera.GetFarPlaneDist());

	shader.SetUniformMat4f("u_inv_projMatrix", glm::inverse(camera.GetProjectionMatrix()));
	shader.SetUniformMat4f("u_inv_viewMatrix", glm::inverse(camera.GetViewMatrix()));

	static bool castShadows = false;
	ImGui::Checkbox("Cast Shadows", &castShadows);

}
