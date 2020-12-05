#pragma once
#include "Core/Layer.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/EntityComponents.h"
#include "ImGuiManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/Buffer/FrameBuffer.h"

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
	FrameBuffer m_SceneFrame;
	ImVec2 m_ImGuiRegionSize;
	ImGuizmo::OPERATION m_TransformWidgetOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE m_TransformWidgetMode = ImGuizmo::MODE::LOCAL;

	EditorLayer(meme::Editor* editor);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
	void DrawDebugVisuals(float dt);
	void DrawScene(const float dt);
	void DrawGizmos(const float dt);
	void DrawCameraInspector(float dt);
	void OnInput(const float dt);
	void EnableDockSpace();
	//void DrawMenuBar();
	void DrawHierarchyPanel();
	void DrawInspectorPanel();
	Camera& GetEditorCamera();

	void DrawGizmoMenu();


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

			auto rotInDeg = glm::degrees(t.Rotation);
			DrawVec3Component("Rotation", rotInDeg);
			t.Rotation = glm::radians(rotInDeg);
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
		ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.4f, 0.3f, 0.4f, 0.8f });
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None))
		{
			ImGui::RadioButton("Initialized", mc.Initialized);
			const std::string  meshStr = fmt::format("Mesh index: {0}", std::to_string(mc.MeshIdx));
			ImGui::Text(meshStr.c_str());

			Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
			const uint32_t nrTextures = static_cast<uint32_t>(mesh.m_Textures.size());

			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			if (ImGui::TreeNode("Textures", "Textures (%i)", nrTextures))
			{
				for (uint32_t i = 0; i < nrTextures; i++)
				{
					ImGui::PushID(i);
					const Texture2D tex = mesh.m_Textures[i];
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					//ImGui::Text("Texture type: %s ", tex.GetType().c_str());

					if (ImGui::TreeNode("Texture", "Type: %s", tex.GetType().c_str(), ImGuiTreeNodeFlags_None))
					{
						ImGui::Text("Width: %i. Height: %i. Render ID: %i", tex.GetWidth(), tex.GetHeight(), tex.GetID());
						const uint32_t id = tex.GetID();

						ImGui::SetNextItemOpen(true, ImGuiCond_Once);
						const ImVec2 availSize = ImGui::GetContentRegionAvail();
						const float aspect = static_cast<float>(tex.GetHeight()) / static_cast<float>(tex.GetWidth());

						ImVec2 thumbSize(0.95f * availSize.x, 0.95f * availSize.x * aspect);
						static bool overlayUVs = false;
						ImGui::SameLine();
						ImGui::Columns();
						ImGui::Checkbox("Overlay UVs", &overlayUVs);
						auto* texPtr = reinterpret_cast<void*>(static_cast<intptr_t>(id));
						if (ImGui::ImageButton(texPtr, thumbSize, ImVec2(0, 1), ImVec2(1, 0), 0))
						{
							ImGui::OpenPopup("texture_popup");
						}
						if (ImGui::BeginPopup("texture_popup"))
						{
							ImGui::Text(("File path: /" + tex.GetPath()).c_str());
							ImGui::EndPopup();
						}
						const auto start = ImGui::GetCursorScreenPos();
						auto sp = ImGui::GetCursorStartPos();
						ImDrawList* drawList = ImGui::GetWindowDrawList();
						const ImVec2 p0 = ImGui::GetItemRectMin();
						const ImVec2 p1 = ImGui::GetItemRectMax();
						const glm::vec2 p0v(p0.x, p0.y);
						const glm::vec2 p1v(p1.x, p1.y);
						ImU32 col_a = ImGui::GetColorU32(IM_COL32(0, 0, 0, 255));
						ImU32 col_b = ImGui::GetColorU32(IM_COL32(255, 255, 255, 255));
						//drawList->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);

						const glm::vec2 scale = { p1.x - p0.x, p1.y - p0.y };
						//if (!mesh.m_Indices.empty())
	
						//draw UVs
						if (!mesh.m_UVs.empty() && overlayUVs && i == 0)
						{
							for (auto face : mesh.m_UVs)
							{
								auto uv1 = glm::mod(face[0], 1.0f);	 uv1.y = 1.0f - uv1.y;
								auto uv2 = glm::mod(face[1], 1.0f);	 uv2.y = 1.0f - uv2.y;
								auto uv3 = glm::mod(face[2], 1.0f);	 uv3.y = 1.0f - uv3.y;

								const auto uv1Scaled = p0v + uv1 * scale;
								const auto uv2Scaled = p0v + uv2 * scale;
								const auto uv3Scaled = p0v + uv3 * scale;

								const ImVec2 v1(uv1Scaled.x, uv1Scaled.y);
								const ImVec2 v2(uv2Scaled.x, uv2Scaled.y);
								const ImVec2 v3(uv3Scaled.x, uv3Scaled.y);

								drawList->AddTriangle(v1, v2, v3, IM_COL32(255, 255, 255, 200), 1);
							}
						}

						ImGui::TreePop();
					}

					ImGui::PopID();
				}

				ImGui::TreePop();
			}
		}

		ImGui::Separator();
		ImGui::PopStyleColor();
	}
};
