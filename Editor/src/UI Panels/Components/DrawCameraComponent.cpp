#include "UI Panels/UI includes.h"

void DrawUIComponent(const glm::mat4& m, const  std::string& label, const float dt);



void DrawUIComponent(CameraComponent& cc, entt::registry& registry, entt::entity entity, const float dt)
{
	auto& transf = registry.get<TransformComponent>(entity);
	//ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	//if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None))

	//TODO FIX THIS!
	auto& camPos = cc.camera.GetPosition();

	auto tmat = transf.CalcMatrix();
	const glm::vec3 viewVec = tmat * glm::vec4(0, 0, -1, 0);
	const glm::vec3 upVec = tmat * glm::vec4(0, 1, 0, 0);
	cc.camera.SetViewVector(viewVec);
	cc.camera.SetUpVector(upVec);
	camPos = transf.Position;

	//ImGui::SameLine();
	float& fov = cc.camera.GetFieldOfView();
	ImGui::SliderFloat("FOV", &fov, 1, 179, "%.2f");
	ImGui::SliderFloat("Near", &cc.camera.GetNearPlaneDist(), 0.01f, 100.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat("Far", &cc.camera.GetFarPlaneDist(), 0.1f, 2000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	ImGui::Text("Aspect: %.3f", cc.camera.GetAspectRatio());

	const auto viewMat = cc.camera.GetViewMatrix();
	const auto proj = cc.camera.GetProjectionMatrix();

	DrawUIComponent(viewMat, "View Matrix", dt);
	DrawUIComponent(proj, "Projection Matrix", dt);
	//	ImGui::TreePop();

	cc.camera.RecalcProjection();
	ImGui::Separator();
}
