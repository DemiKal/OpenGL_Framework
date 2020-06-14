#include "precomp.h"
#include "PostProcessing.h"

#include "ScreenQuad.h"
#include "ShaderManager.h" 
#include "Light/LightManager.h"


void PostProcessing::ShadowCastGLSL(Camera& camera, Gbuffer& gBuffer)
{
	Shader& shader = ShaderManager::GetShader("shadow_cast");
	static bool castShadows = false;
	ImGui::Checkbox("Cast Shadows", &castShadows);
	float ambient = LightManager::GetAmbientLight();
	shader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetPositionID());
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetNormalID());
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, gBuffer.GetAlbedoSpecID());


	const glm::vec3& lightDir = LightManager::GetDirectionalLight();
	shader.SetVec3f("u_globalLightDir", lightDir);
	shader.SetFloat("u_ambientLight", ambient);
	shader.SetFloat("u_screenWidth", (float)SCREENWIDTH);
	shader.SetFloat("u_screenWidth", (float)SCREENHEIGHT);
	shader.SetFloat("u_nearPlane", camera.GetNearPlaneDist());
	shader.SetFloat("u_nearPlane", camera.GetFarPlaneDist());
	shader.SetUniformMat4f("u_inv_projMatrix", glm::inverse(camera.GetProjectionMatrix()));
	shader.SetUniformMat4f("u_inv_viewMatrix", glm::inverse(camera.GetViewMatrix()));
	shader.SetInt("u_shadowCast", castShadows);
	shader.SetVec3f("u_cameraPos", camera.GetPosition());

 

	const char* listbox_items[] = { "wpos", "normal", "albedo" };
	static int displayMode = 0;
	ImGui::ListBox("render pass 2\n(single select)", &displayMode, listbox_items, IM_ARRAYSIZE(listbox_items), 5);

	
	shader.SetInt("renderMode", displayMode);

	//ScreenQuad::Bind();
	ScreenQuad::Draw();
}
