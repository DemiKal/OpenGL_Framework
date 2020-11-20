#include "EditorLayer.h"
#include "ImGuiManager.h"
//#include "GameObject/EntityManager.h"
#include "Editor.h"
#include "Rendering/Renderer.h"
#include "GameObject/Components/EntityComponents.h"

void EditorLayer::OnAttach()
{
	fmt::print("on attach!");

	auto e = m_Registry.create();
	m_Registry.emplace<TransformComponent>(e);
	m_Registry.emplace<TagComponent>(e, "spyro");
	m_Registry.emplace<MeshComponent>(e, "res/meshes/spyro/spyro.obj", aiProcess_Triangulate);
}

void EditorLayer::OnDetach()
{

}

void EditorLayer::OnUpdate(float dt)
{
	OnInput();
}

void EditorLayer::OnImGuiRender()
{
	static bool yahoo = true;
	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	ImGui::ShowDemoWindow(&yahoo);

	EnableDockSpace();
	//DrawMenuBar();
	DrawEntityPanel();
	DrawInspectorPanel();
}

void EditorLayer::OnInput()
{
	if (glfwGetKey(Renderer::GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		m_Editor->StopRunning();

}

void EditorLayer::EnableDockSpace()
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	static bool p_open = true;
	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
			ImGui::Separator();

			if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
				p_open = false;
			ImGui::EndMenu();
		}
		//HelpMarker(
		//    "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n\n"
		//    " > if io.ConfigDockingWithShift==false (default):" "\n"
		//    "   drag windows from title bar to dock" "\n"
		//    " > if io.ConfigDockingWithShift==true:" "\n"
		//    "   drag windows from anywhere and hold Shift to dock" "\n\n"
		//    "This demo app has nothing to do with it!" "\n\n"
		//    "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)." "\n\n"
		//    "ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame." "\n\n"
		//    "(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)"
		//);

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void EditorLayer::DrawEntityPanel()
{
	const auto flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Entities", NULL, flags);
	auto view = m_Registry.view<TagComponent>();
	for (auto entity : view)
	{
		auto name = m_Registry.get<TagComponent>(entity);
		const auto nodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;

		if (ImGui::TreeNodeEx(name.Name, nodeFlags))
		{
			ImGui::Text("Child entity");
			 	ImGui::TreePop();
		}
	}

	ImGui::End();
}

void DrawEntityComponent(TransformComponent& tfc)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Transform"))
	{
		ImGui::SliderFloat3("Position", glm::value_ptr(tfc.Position), -100, 100);
		ImGui::SliderFloat3("Rotation", glm::value_ptr(tfc.Rotation), -100, 100);
		ImGui::SliderFloat3("Scale", glm::value_ptr(tfc.Scale), -100, 100);
		ImGui::TreePop();
	}
}


void DrawEntityComponent(MeshComponent& mc)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Mesh"))
	{
		ImGui::RadioButton("Initialized", mc.Initialized);
		const std::string  meshStr = fmt::format("Mesh index: {0}", std::to_string(mc.MeshIdx));
		ImGui::Text(meshStr.c_str());

		Mesh& mesh = Mesh::m_Meshes[mc.MeshIdx];
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

				ImGui::ImageButton(reinterpret_cast<void*>(static_cast<intptr_t>(id)),
					thumbSize, ImVec2(0, 1), ImVec2(1, 0));


				//ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
void EditorLayer::DrawInspectorPanel()
{
	auto view = m_Registry.view<TransformComponent>();

	ImGui::Begin("Inspector");
	for (auto entity : view)
	{
		auto& tfc = m_Registry.get<TransformComponent>(entity);
		DrawEntityComponent(tfc);

		auto& mc = m_Registry.get<MeshComponent>(entity);
		DrawEntityComponent(mc);
	}
	ImGui::End();
}


