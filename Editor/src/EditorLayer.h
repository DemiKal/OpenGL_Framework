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

	entt::registry m_Registry;
	meme::Editor* m_Editor;
	Camera m_EditorCamera;

	EditorLayer(meme::Editor* editor) :
		Layer("EditorLayer"),
		m_Editor(editor),
		m_EditorCamera(glm::vec3(0, 3, 16), 70, static_cast<float>(SCREENWIDTH) / static_cast<float>(SCREENHEIGHT), 0.1f, 700.0f)
	{

	}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
	void DrawCameraInspector(float dt);
	void OnInput(const float dt);
	void EnableDockSpace();
	//void DrawMenuBar();
	void DrawEntityPanel();
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
		if (ImGui::TreeNode(label.c_str()))
		{
			DrawVec3Component("Translation", t.Position);
			DrawVec3Component("Rotation", t.Rotation);
			DrawVec3Component("Scale", t.Scale, 1.0f);
			ImGui::TreePop();
		}
	}

	void DrawUIComponent(MeshComponent& mc, const std::string& label)
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode(label.c_str()))
		{
			ImGui::RadioButton("Initialized", mc.Initialized);
			const std::string  meshStr = fmt::format("Mesh index: {0}", std::to_string(mc.MeshIdx));
			ImGui::Text(meshStr.c_str());

			Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
			const uint32_t nrTextures = mesh.m_Textures.size();

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

					ImVec2 thumbSize(availSize.x, availSize.x * aspect);

					if (ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(id)), thumbSize, ImVec2(0, 1), ImVec2(1, 0)))
						ImGui::OpenPopup("texture_popup");

					if (ImGui::BeginPopup("texture_popup"))
					{
						ImGui::Text(("File path: /" + tex.GetPath()).c_str());
						ImGui::EndPopup();
					}
				}
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}
};
