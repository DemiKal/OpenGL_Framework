#include "EditorLayer.h"
#include "Editor.h"
#include "GameObject/Components/EntityComponents.h"
#include "Rendering/Renderer.h"
#include "RenderLayer.h"
#include "Gizmos/FrustumGizmo.h"

EditorLayer::EditorLayer(meme::Editor* editor) :
	Layer("EditorLayer"),
	m_Editor(editor),
	m_EditorCamera(glm::vec3(0, 3, 16), 70, static_cast<float>(SCREENWIDTH) / static_cast<float>(SCREENHEIGHT), 0.1f, 700.0f)
{
	m_Gizmos.emplace_back(new FrustumGizmo(this));

}

void EditorLayer::OnAttach()
{
	fmt::print("{} on attach!\n", m_Name);

	const auto cube = m_Registry.create();
	m_Registry.emplace<TransformComponent>(cube);
	m_Registry.emplace<TagComponent>(cube, "Cube");
	//m_Registry.emplace<MeshComponent>(e, "Assets/meshes/spyro/spyro.obj", aiProcess_Triangulate);
	auto& m = m_Registry.emplace<MeshComponent>(cube, MeshComponent::PrimitiveType::CubeWireframe);
	m.ShaderIdx = ShaderManager::GetShaderIdx("AABB_single");
	m_Registry.emplace<CameraComponent>(cube);

	const auto spyro = m_Registry.create();
	m_Registry.emplace<TransformComponent>(spyro);
	m_Registry.emplace<TagComponent>(spyro, "Spyro the Dragon");
	//m_Registry.emplace<MeshComponent>(spyro, "Assets/meshes/DamagedHelmet.glb", aiProcess_Triangulate);
	//m_Registry.emplace<MeshComponent>(spyro, "Assets/meshes/DamagedHelmet.glb", aiProcess_Triangulate);
	m_Registry.emplace<MeshComponent>(spyro, "Assets/meshes/spyro/spyro.obj", aiProcess_Triangulate);
	m_Selected = spyro;

	const auto anim = m_Registry.create();
	m_Registry.emplace<TransformComponent>(anim);
	m_Registry.emplace<TagComponent>(anim, "anim");
	//m_Registry.emplace<MeshComponent>(spyro, "Assets/meshes/DamagedHelmet.glb", aiProcess_Triangulate);
	auto& mc2 = m_Registry.emplace<MeshComponent>(anim, "Assets/meshes/Animation test/run embedded.gltf", aiProcess_Triangulate);

	mc2.ShaderIdx = ShaderManager::GetShaderIdx("anim");


}

void EditorLayer::OnDetach()
{
	m_Registry.clear<>();
}

void EditorLayer::OnUpdate(const float dt)
{
	OnInput(dt);
	RenderScene(dt);

}

void EditorLayer::DrawCameraInspector(const float dt)
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	ImGui::Begin("Rendering debug");
	if (ImGui::TreeNode("Editor Camera debug"))
	{
		const auto camPos = m_EditorCamera.GetPosition();
		const ImVec2 buttonSize = { 25,25 };
		ImGui::Button("X", buttonSize); ImGui::SameLine(); ImGui::Text("%.3f", camPos.x);
		ImGui::SameLine(); ImGui::Button("Y", buttonSize); ImGui::SameLine(); ImGui::Text("%.3f", camPos.y);
		ImGui::SameLine(); ImGui::Button("Z", buttonSize); ImGui::SameLine(); ImGui::Text("%.3f", camPos.z);
		//ImGui::SameLine();
		float& fov = m_EditorCamera.GetFieldOfView();
		ImGui::SliderFloat("FOV", &fov, 1, 179, "%.2f");
		ImGui::SliderFloat("Near", &m_EditorCamera.GetNearPlaneDist(), 0.01f, 100.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Far", &m_EditorCamera.GetFarPlaneDist(), 0.1f, 2000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
		ImGui::Text("Aspect: %.3f", m_EditorCamera.GetAspectRatio());

		const auto viewMat = m_EditorCamera.GetViewMatrix();
		const auto proj = m_EditorCamera.GetProjectionMatrix();

	//	DrawUIComponent(viewMat, "View Matrix", dt);
	//	DrawUIComponent(proj, "Projection Matrix", dt);
		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Render statistics"))
	{
		ImGui::Text("ms per frame: %.3f", static_cast<double>(dt) * 1000.0);
		ImGui::Text("fps: %.3f", 1.0 / static_cast<double>(dt));
		ImGui::TreePop();
	}


	ImGui::End();
	m_EditorCamera.RecalcProjection();
}

void EditorLayer::OnInput(const float dt)
{
	auto* window = m_Editor->GetRenderer().GetWindow();
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		m_Editor->StopRunning();

	const float camSpeed = 25.0f; ;
	const glm::vec3 fw = camSpeed * m_EditorCamera.GetForwardVector();
	const glm::vec3 up = camSpeed * m_EditorCamera.GetUpVector();
	const glm::vec3 left = camSpeed * normalize(glm::cross(up, fw));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_EditorCamera.GetPosition() += dt * fw;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_EditorCamera.GetPosition() += dt * left;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_EditorCamera.GetPosition() += dt * -left;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_EditorCamera.GetPosition() += dt * -fw;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		m_EditorCamera.GetPosition() += dt * up;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		m_EditorCamera.GetPosition() += dt * -up;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m_EditorCamera.RotateLocalY(-1.0f * dt);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_EditorCamera.RotateLocalY(1.0f * dt);
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

	if (ImGui::BeginMainMenuBar())
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

		ImGui::EndMainMenuBar();
	}

	DrawGizmoMenu();

	ImGui::End();
}

Camera& EditorLayer::GetEditorCamera()
{
	return m_EditorCamera;
}

void EditorLayer::DrawGizmoMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("##test", false))
		{
			ImGui::EndMenu();
		}

		static bool what = true;
		//ImGui::SameLine();
		//ImGui::Checkbox("aa", &what);
		auto& w = m_TransformWidgetOperation;
		using o = ImGuizmo::OPERATION;

		//ImGui::setnextpos
		ImGui::SameLine(ImGui::GetContentRegionAvail().x / 3);
		static ImVec2 minGrp{ 0, 0 };
		static ImVec2 maxGrp{ 0, 0 };

		{

			ImGui::GetWindowDrawList()->AddRectFilled(minGrp, maxGrp, IM_COL32(0, 12, 23, 190));

			ImGui::BeginGroup();

			if (ImGui::RadioButton("Translation", w == o::TRANSLATE)) w = o::TRANSLATE;
			if (ImGui::RadioButton("Rotation", w == o::ROTATE)) w = o::ROTATE;
			if (ImGui::RadioButton("Scale", w == o::SCALE)) w = o::SCALE;

			auto& m = m_TransformWidgetMode;
			using t = ImGuizmo::MODE;
			if (ImGui::RadioButton("Local", m == t::LOCAL)) m = t::LOCAL;
			if (ImGui::RadioButton("World", m == t::WORLD)) m = t::WORLD;
			ImGui::EndGroup();

			minGrp = ImGui::GetItemRectMin() * 0.99f;
			maxGrp = ImGui::GetItemRectMax() * 1.01f;
		}

		ImGui::EndMenuBar();
	}
}

void EditorLayer::OnImGuiRender(const float dt)
{
	static bool yahoo = true;
	ImGui::ShowDemoWindow(&yahoo);

	EnableDockSpace();
	RenderViewportPanel(dt);
	RenderInspectorPanel(dt);
	RenderSceneHierarchyPanel(dt);

	DrawCameraInspector(dt);
	DrawDebugVisuals(dt);
}

//TODO: delegate debug visuals to the components?
void EditorLayer::DrawDebugVisuals(float dt)
{
	m_SceneFrame.Bind();

	for (auto& gizmo : m_Gizmos)
		if (gizmo->m_Enabled) gizmo->Draw();

	m_SceneFrame.Unbind();
}

void EditorLayer::RenderSkybox()
{
	m_Editor->GetRenderer().SetDepthFunc(GL_LEQUAL);

	Camera& cam = GetEditorCamera();
	Mesh* skybox = MeshManager::GetMesh(Meshtype::Skybox);
	Shader& shader = ShaderManager::GetShader("skybox");
	shader.Bind();
	auto view = glm::mat4(glm::mat3(cam.GetViewMatrix()));
	auto proj = cam.GetProjectionMatrix();
	auto camPos = cam.GetPosition();
	auto translMat = (glm::translate(glm::mat4(1.0f), camPos));

	skybox->Draw(proj, view, shader);

	shader.Unbind();
	m_Editor->GetRenderer().SetDepthFunc(GL_LESS);

}

void EditorLayer::RenderScene(const float dt)
{
	Renderer& renderer = m_Editor->GetRenderer();
	m_SceneFrame.Bind();

	auto [fbWidth, fbHeight] = m_SceneFrame.GetSize();
	auto prevViewport = renderer.GetViewPort();
	renderer.SetViewPort(0, 0, fbWidth, fbHeight);

	if ((fbWidth != static_cast<uint32_t>(m_ImGuiRegionSize.x) || fbHeight != static_cast<uint32_t>(m_ImGuiRegionSize.y)) && m_ImGuiRegionSize.x > 0 && m_ImGuiRegionSize.y > 0)
	{
		const float aspect = m_ImGuiRegionSize.x / m_ImGuiRegionSize.y;
		m_EditorCamera.SetAspectRatio(aspect);
	}
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	renderer.SetClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	renderer.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderer.EnableDepth();
	renderer.SetAlphaBlending(true);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD); //allows us to set this operationand has 5 possible options :

	auto view = m_Registry.view<MeshComponent, TransformComponent>();

	for (auto entity : view)
	{
		auto [mc, transform] = m_Registry.get<MeshComponent, TransformComponent>(entity);
		const uint32_t idx = mc.MeshIdx;
		Mesh& mesh = MeshManager::GetMesh(idx);
		Shader& shader = ShaderManager::GetShader(mc.ShaderIdx);
		const glm::mat4 mat = transform.CalcMatrix();

		if (mc.Enabled)	mesh.Draw(m_EditorCamera, mat, shader);

		if (mc.DrawWireFrame)
		{
			bool a = renderer.GetAlphaBlending();
			renderer.SetAlphaBlending(true);
			mesh.DrawWireFrame(m_EditorCamera, mat, mc.WireFrameColor);
			renderer.SetAlphaBlending(a);

		}

		if (mc.DrawNormals)
		{
			bool a = renderer.GetAlphaBlending();
			renderer.SetAlphaBlending(true);
			mesh.DrawNormals(m_EditorCamera, mat, mc.NormalsColor, mc.NormalsMagnitude);
			renderer.SetAlphaBlending(a);

		}
	}
	RenderSkybox();

	m_SceneFrame.Unbind();
	//renderer.SetViewPort(prevViewport);





}


void EditorLayer::DrawGizmos(const float dt)
{

	if (m_Selected == entt::null) return;

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();
	auto windowPos = ImGui::GetWindowPos();
	auto vporS = ImGui::GetWindowViewport()->Pos;
	auto vporW = ImGui::GetWindowViewport()->Size;
	//auto diff = windowHeight - m_ImGuiRegionSize.y; //TODO: check if this really fixes it!
	ImGuizmo::SetRect(windowPos.x, windowPos.y, m_ImGuiRegionSize.x, m_ImGuiRegionSize.y);

	auto projMat = m_EditorCamera.GetProjectionMatrix();
	auto viewMat = m_EditorCamera.GetViewMatrix();
	auto avail = ImGui::GetContentRegionAvail();
	TransformComponent& tc = m_Registry.get<TransformComponent>(m_Selected);
	glm::mat4 transform = tc.CalcMatrix();//tc.Position

	if (m_TransformWidgetOperation == ImGuizmo::OPERATION::SCALE)
		m_TransformWidgetMode = ImGuizmo::MODE::LOCAL;

	 ImGuizmo::Manipulate(glm::value_ptr(viewMat), glm::value_ptr(projMat),
	 	m_TransformWidgetOperation, m_TransformWidgetMode, glm::value_ptr(transform));

	if (m_Registry.has<MeshComponent>(m_Selected))
	{
		MeshComponent& mc = m_Registry.get<MeshComponent>(m_Selected);
		Mesh& mesh = MeshManager::GetMesh(mc.MeshIdx);
		if (mesh.HasAnimation())
		{
			Camera& cam = GetEditorCamera();
			const glm::mat4 vp = cam.GetViewProjectionMatrix();

			glm::vec4 screenpos = vp * glm::vec4(tc.Position, 1.0f);
			glm::vec4 scrpw = screenpos / screenpos.w;

 
			//for (Joint& joint : mesh.m_animator.m_Bones)
			//{
			//	//glm::mat4 transf = glm::translate(glm::mat4(1.0f), glm::vec3(joint.m_PoseTransform ));
			//	glm::mat4 tp = glm::mat4(1.0f);
			//	glm::vec3 jt = {};
			//	glm::vec3 jr = {};
			//	glm::vec3 js = {};
			//
			//	ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(joint.m_PoseTransform), &jt[0], &jr[0], &js[0]);
			//	//glm::mat4 transf =  glm::inverse(joint.m_Offset) * glm::inverse(joint.m_PoseTransform);
			//	glm::mat4 transf =   glm::inverse(joint.m_Offset) * glm::translate(glm::mat4(1.0f), jt)  /*glm::inverse*/ ;
			//
			//	ImGuizmo::Manipulate(glm::value_ptr(viewMat), glm::value_ptr(projMat),
			//		m_TransformWidgetOperation, m_TransformWidgetMode, glm::value_ptr(transf));
			//}


			auto drawlist = ImGui::GetForegroundDrawList();

			auto avai0 = ImGui::GetContentRegionAvail();
			auto avail2 = ImGui::GetWindowSize();
			auto avail3 = ImGui::GetWindowViewport();

			//drawlist->AddRectFilled({ 50,50 }, { 100, 100 }, IM_COL32_WHITE);

		}
	}






	if (ImGuizmo::IsUsing())
	{
		glm::vec3 transl, rot, scale, skew;
		glm::vec4  persp;
		glm::quat orientation;

		glm::decompose(transform, scale, orientation, transl, skew, persp);

		glm::vec3 fromQuat = glm::eulerAngles(orientation);

		glm::vec3 deltaRot = fromQuat - tc.Rotation;
		tc.Position = transl;
		tc.Rotation += deltaRot;
		tc.Scale = scale;
	}

}