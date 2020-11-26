#pragma once
#include "Core/layer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace meme {
	class Editor;
}

class EditorLayer : public Layer
{

public:
	meme::Editor* m_Editor;
	entt::registry m_Registry;
	entt::entity m_Selected{ entt::null };
	Camera m_EditorCamera;

	EditorLayer(meme::Editor* editor);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
	void DrawCameraInspector(float dt);
	void OnInput(const float dt);
	void EnableDockSpace();
	//void DrawMenuBar();
	void DrawHierarchyPanel();
	void DrawInspectorPanel();
	Camera& GetEditorCamera();


	template<typename T>
	void DrawUIComponent(T t)
	{
		static_assert(false);
	}

	void DrawVec3Component(const std::string& label, glm::vec3& vec, float resetVal = 0)
	{
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 1 ,1 });

		if (ImGui::Button("X")) vec.x = resetVal;

		ImGui::SameLine();
		ImGui::DragFloat("##X", &vec.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("Y")) vec.y = resetVal;

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &vec.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("Z")) vec.z = resetVal;

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &vec.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopStyleVar();
		ImGui::PopID();
	}

	void DrawUIComponent(TransformComponent& t, const std::string& label)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None))
		{
			DrawVec3Component("Translation", t.Position);
			DrawVec3Component("Rotation", t.Rotation);
			DrawVec3Component("Scale", t.Scale, 1.0f);
			//	ImGui::TreePop();
		}
		ImGui::Separator();
	}

	void DrawUIComponent(const glm::mat4& m, const  std::string& label)
	{
		// ImGui::TreeNodeEx("matrix",ImGuiTreeNodeFlags_::)
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode(label.c_str()))
		{
			ImGui::Text("%.2f", m[0].x); ImGui::SameLine();
			ImGui::Text("%.2f", m[0].y); ImGui::SameLine();
			ImGui::Text("%.2f", m[0].z);

			ImGui::Text("%.2f", m[1].x); ImGui::SameLine();
			ImGui::Text("%.2f", m[1].y); ImGui::SameLine();
			ImGui::Text("%.2f", m[1].z);

			ImGui::Text("%.2f", m[2].x); ImGui::SameLine();
			ImGui::Text("%.2f", m[2].y); ImGui::SameLine();
			ImGui::Text("%.2f", m[2].z);

			ImGui::Text("%.2f", m[3].x); ImGui::SameLine();
			ImGui::Text("%.2f", m[3].y); ImGui::SameLine();
			ImGui::Text("%.2f", m[3].z);
			ImGui::TreePop();
		}
	}
	void DrawUIComponent(CameraComponent& cc, const std::string& label, entt::entity entity)
	{
		auto& transf = m_Registry.get<TransformComponent>(entity);
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None))
		{
			//TODO FIX THIS!
			auto& camPos = cc.camera.GetPosition();
			//camPos = pos;

			const ImVec2 buttonSize = { 20,20 };
			//ImGui::Button("X", buttonSize); ImGui::SameLine(); ImGui::Text("%.3f", camPos.x);
			//ImGui::SameLine(); ImGui::Button("Y", buttonSize); ImGui::SameLine(); ImGui::Text("%.3f", camPos.y);
			//ImGui::SameLine(); ImGui::Button("Z", buttonSize); ImGui::SameLine(); ImGui::Text("%.3f", camPos.z);

			//DrawVec3Component("Translation", camPos);

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

			DrawUIComponent(viewMat, "View Matrix");
			DrawUIComponent(proj, "Projection Matrix");
			//	ImGui::TreePop();
		}
		cc.camera.RecalcProjection();
		ImGui::Separator();
	}

	void DrawUIComponent(MeshComponent& mc, const std::string& label)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, { 0.4,0.3,0.4,0.8 });
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None))
		{
			//if (ImGui::BeginChild("Mesh", { 0,0 }, true))
			//{
			ImGui::RadioButton("Initialized", mc.Initialized);
			const std::string  meshStr = fmt::format("Mesh index: {0}", std::to_string(mc.MeshIdx));
			ImGui::Text(meshStr.c_str());

			Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
			const uint32_t nrTextures = static_cast<uint32_t>(mesh.m_Textures.size());

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Textures"))
			{
				for (uint32_t i = 0; i < nrTextures; i++)
				{
					const Texture2D tex = mesh.m_Textures[i];
					const uint32_t id = tex.GetID();

					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					ImGui::Text("Texture type: %s ", tex.GetType().c_str());
					const ImVec2 availSize = ImGui::GetContentRegionAvail();
					const float aspect = static_cast<float>(tex.GetHeight()) / static_cast<float>(tex.GetWidth());

					ImVec2 thumbSize(0.95f * availSize.x, 0.95f * availSize.x * aspect);

					if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(id)), thumbSize, ImVec2(0, 1), ImVec2(1, 0)))
					{
						ImGui::OpenPopup("texture_popup");
					}
					if (ImGui::BeginPopup("texture_popup"))
					{
						ImGui::Text(("File path: /" + tex.GetPath()).c_str());
						ImGui::EndPopup();
					}
					const auto start = ImGui::GetCursorScreenPos();

					ImDrawList* drawList = ImGui::GetWindowDrawList();
					const ImVec2 p0 = ImGui::GetItemRectMin();
					const ImVec2 p1 = ImGui::GetItemRectMax();
					const glm::vec2 p0v(p0.x, p0.y);
					const glm::vec2 p1v(p1.x, p1.y);

					const glm::vec2 scale = { p1.x - p0.x, p1.y - p0.y };
					//if (!mesh.m_Indices.empty())
					if (!mesh.m_UVs.empty())
						//for (size_t uvIdx = 3; uvIdx < mesh.m_UVs.size(); uvIdx += 3)
							//for (size_t index : mesh.m_Indices)
						for (auto& face : mesh.m_UVs)
						{
							auto uv1 = face[0];	 uv1.y = 1.0f - uv1.y;
							auto uv2 = face[1];	 uv2.y = 1.0f - uv2.y;
							auto uv3 = face[2];	 uv3.y = 1.0f - uv3.y;

							auto uv1Scaled = p0v + uv1* scale;
							auto uv2Scaled = p0v + uv2* scale;
							auto uv3Scaled = p0v + uv3* scale;

							ImVec2 v1(uv1Scaled.x, uv1Scaled.y);
							ImVec2 v2(uv2Scaled.x, uv2Scaled.y);
							ImVec2 v3(uv3Scaled.x, uv3Scaled.y);
							//auto  = p0v + uv1Scaled;

							drawList->AddLine(v1, v2, IM_COL32(255, 255, 255, 200), 2);
							drawList->AddLine(v1, v3, IM_COL32(255, 255, 255, 200), 2);
							drawList->AddLine(v2, v3, IM_COL32(255, 255, 255, 200), 2);

							//const float u1Normalized = mesh.m_UVs[uvIdx - 3l];
								//const float v1Normalized = 1.0f - mesh.m_UVs[uvIdx - 2l];
								//const float u2Normalized = mesh.m_UVs[uvIdx - 1l];
								//const float v2Normalized = 1.0f - mesh.m_UVs[uvIdx];
								//
								//const float u1 = p0.x + u1Normalized * scale.x;
								//const float v1 = p0.y + v1Normalized * scale.y;
								//const float u2 = p0.x + u2Normalized * scale.x;
								//const float v2 = p0.y + v2Normalized * scale.y;

							//drawList->AddLine({ u1,v1 }, { u2,v2 }, IM_COL32(255, 255, 255, 200), 2);
							//drawList->AddLine(p0, p1, IM_COL32(255, 255, 255, 200), 2);
						}
				}
				//ImGui::Separator();
				ImGui::TreePop();
			}
		}

		ImGui::Separator();
		ImGui::PopStyleColor();
	}
};
