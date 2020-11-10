#define STB_IMAGE_IMPLEMENTATION
#include "precomp.h"

#include "Rendering/Buffer/FrameBuffer.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/Buffer/Gbuffer.h"
#include "GameObject/Camera.h"
#include "Light/LightManager.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/Model.h"
#include "misc/InputManager.h"	
#include "Geometry/BVH/BVH.h"
#include "misc/UserInterface.h"
#include "Rendering/PostProcessing.h"
#include <GameObject\Components\EntityComponents.h>

//#include <src\vendor\imgui\imgui.h>

// #include <src/vendor/imgui/imgui.h>
// #include <src/vendor/imgui/imgui_impl_glfw.h>
// #include <src/vendor/imgui/imgui_impl_opengl3.h> 
 //#include <src/vendor/imgui/extensions/imGuIZMOquat.h> 
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h> 

#ifndef _DEBUG
#define BUNNY
#define DRAGON
#endif

void ShowDockWindow()
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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
	static bool p_open = true;
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
	else
	{
		//ShowDockingDisabledMessage();
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

		ImGui::EndMenuBar();
	}

	ImGui::End();


}
void PrintMessage()
{
	fmt::print("hi there m8!\n");
}
int main(void)
{
	if (!glfwInit()) return -1;


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE

	//glfwWindowHint(GLFW_FULLSCREEN, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// glfwGetPrimaryMonitor() ;

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", nullptr, nullptr);

	if (!window)
	{
		glfwTerminate();
		throw std::exception("ERROR: Could not create GLFW window!");
		return -1;
	}

	InputManager::SetWindow(window);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) fmt::print("ERROR!\n");

	HardwareQuery::Query();
	{
		Renderer renderer;

		//init before any model
		ShaderManager::Init();
		UserInterface userInterface;
		Shader& agnosticShader = ShaderManager::GetShader("Gbuffer_basic_vertex");
		Model spyro("res/meshes/Spyro/Spyro.obj", aiProcess_Triangulate);
		spyro.SetShader("Gbuffer_basic");
		spyro.m_name = "Spyro";
		EntityManager::AddEntity(spyro);

		EntityManager::Init();

		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		artisans.SetShader("Gbuffer_basic");
		artisans.m_name = "artisans";
		EntityManager::AddEntity(artisans);

#ifdef BUNNY
		Model bunny("res/meshes/erato/erato.obj", aiProcess_Triangulate);
		bunny.SetShader("Gbuffer_basic");
		bunny.m_name = "Erato";
		EntityManager::AddEntity(bunny);
#endif
#ifdef DRAGON
		Model dragon("res/meshes/black dragon/blackdragon.obj", aiProcess_Triangulate);
		dragon.SetShader("Gbuffer_basic");
		dragon.m_name = "Dragon";
		EntityManager::AddEntity(dragon);
#endif

		BVH bvh;
		bvh.BuildBVH(renderer);

		FrameBuffer frameBuffer;
		FrameBuffer shadowMap;


		// configure g-buffer framebuffer
		// ------------------------------
		Gbuffer gBuffer;
		gBuffer.SetShader("deferred_shading");
		gBuffer.BindShader();
		//deferredShading.Bind();

		const float aspect = static_cast<float>(SCREENWIDTH) / static_cast<float>(SCREENHEIGHT);
		const auto originalCamPos = glm::vec3(0, 3, 16);
		Camera camera(originalCamPos, 70, aspect, 0.1f, 700.0f);
		glm::vec3 dirLightPos = glm::vec3(0, 0, 0);

		Camera shadowCam(dirLightPos, 70, aspect, 0.1f, 700.0f);
		//shadowCam.RotateLocalY(180);
		//shadowCam.RotateLocalX(25);

		float orthoW = 20.0f;
		float orthoH = 40.0f;
		float orthoN = 0;
		float orthoF = 40.0f;


		//	shadowCam.RotateLocalX(glm::radians( 90.0f));

		Camera::SetMainCamera(&camera);
		Camera::SetCamera2(&shadowCam);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		auto dockspace_flags = ImGuiDockNodeFlags_::ImGuiDockNodeFlags_None;
		//if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		//{
		//	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		//	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		//}


		//ImGui::ShowExampleAppDockSpace(&dock);
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		const char* glsl_version = "#version 130";
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);


		//ScreenQuad screenQuad;

		LightManager::AddLight({ 0, 5, 0 }, { 1, 0, 0 });
		LightManager::AddLight({ -2, 3, 0 }, { 0, 1, 0 });
		LightManager::AddLight({ 2, 3, 0 }, { 0, 0, 1 });

		renderer.m_prevFrameTime = (float)glfwGetTime();

		bool drawBvh = false;

		//glm::vec3 dirLightPos = glm::vec3(0.0f);

		float nearLight = 0.1f;
		float farLight = 100.0f;
		float camLR = 10.0f;
		float camUD = 10.0f;
		glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearLight, farLight);
		//glm::mat4 lightProj = glm::perspective(70.0f, aspect, nearLight, farLight);

		shadowCam.GetPosition() = { 0,20,-10 };

		int shadowWidth = 1024;
		int shadowHeight = 1024;
		//double totalTime = 0;
		//float rotation = 0;
		int fbw, fbh, w, h;
		glfwGetFramebufferSize(window, &fbw, &fbh);
		glfwGetWindowSize(window, &w, &h);


		entt::registry registry;

		glm::vec3 pos(0, 0, 0);
		glm::vec3 vel(0, -9.81, 0);
		//vec3 scale(1, 1, 1);
		struct DirComponent
		{
			glm::vec3 dir;
		};
		const int count = 3;
		std::array<glm::vec3, 3> arr{
			glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{ 0,1,0 }, glm::vec3{ 0,0,1 } };

		for (int i = 0; i < count; i++)
		{
			auto entity = registry.create();
			auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1, 0, 0));
			registry.emplace<TransformComponent>(entity, trans);
			entt::hashed_string nm{ "particle " + count };
			registry.emplace<NameComponent>(entity, nm);
			registry.emplace<DirComponent>(entity, arr[i]);

			auto& t = registry.get<TransformComponent>(entity);
			auto& taa = registry.get<NameComponent>(entity).Name;

		}

		long frame = 0;
		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			//ImGui::ShowDemoWindow();
			//ShowDockWindow();
			ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

			renderer.m_currentFrameTime = static_cast<float>(glfwGetTime());
			float deltaTime = static_cast<float>(renderer.m_currentFrameTime - renderer.m_prevFrameTime);

			Renderer::ClearColor(1, 1, 1, 1);
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::EnableDepth();
			renderer.SetDepthFunc(GL_LEQUAL);
			renderer.SetCullingMode(GL_BACK);
			renderer.SetAlphaBlending(false);

#pragma region input
			userInterface.Update(deltaTime);
			InputManager::Update(camera, deltaTime);
			renderer.UpdateUI(deltaTime);

#pragma endregion input
			//update meshes
			artisans.Update(deltaTime);
			spyro.Update(deltaTime);

#if defined (BUNNY) && defined(DRAGON) 
			bunny.Update(deltaTime);
			dragon.Update(deltaTime);
#endif

			Renderer::EnableDepth();
			gBuffer.Bind();
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
			//camera.SetViewVector(shadowCam.GetForwardVector());
			//camera.GetPosition() = shadowCam.PositionRead();

			// deferred pass
			//draw meshes regularly

			//artisans.Draw(camera);
			//spyro.Draw(camera);

			auto view = registry.view<const TransformComponent, NameComponent>();
			for (auto entity : view)
			{
				auto xx = view.size();
				auto tra = registry.get<TransformComponent>(entity).Transform;
				auto dir = registry.get<DirComponent>(entity).dir;
				auto namee = registry.get<NameComponent>(entity).Name;

				//ImGui::LabelText("lbl",  "weewwoo");

				tra = glm::rotate(tra, renderer.m_currentFrameTime, dir);

				spyro.m_modelMatrix = tra;
				spyro.Draw(camera);
			}

#if defined (BUNNY) && defined(DRAGON)
			bunny.Draw(camera);
			dragon.Draw(camera);
#endif	
			glm::vec3& lightDir = LightManager::GetDirectionalLight();
			//SHADOW PASS

			ImGui::Begin("meme");
			ImGui::SliderFloat("near Light", &nearLight, 0, farLight);
			ImGui::SliderFloat("far Light", &farLight, nearLight, 500);
			ImGui::SliderFloat3("dir light position", glm::value_ptr(dirLightPos), -500, 500);
			ImGui::End();
			//shadowCam.Roll(0.5f *  deltaTime);

			shadowCam.SetOrthographic(orthoW, orthoH, orthoN, orthoF);
			shadowCam.GetPosition() = dirLightPos;

			shadowMap.Bind();
			renderer.EnableDepth();
			renderer.SetCullingMode(GL_FRONT);
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Renderer::ClearColor(1, 1, 1, 1);
			artisans.Draw(shadowCam, agnosticShader);
			spyro.Draw(shadowCam, agnosticShader);
			//renderer.SetCullingMode(GL_BACK);

#ifdef BUNNY 
			bunny.Draw(cam2);
#endif
#ifdef DRAGON
			dragon.Draw(cam2);
#endif
			renderer.SetCullingMode(GL_BACK);

			ImGui::Begin("meme");
			ImGui::SliderFloat("cam 2 width", &orthoW, 0.001f, 400.0f);
			ImGui::SliderFloat("cam 2 height", &orthoH, 0.001f, 400.0f);
			ImGui::SliderFloat("cam 2 near", &orthoN, -400.0f, 400.0f);
			ImGui::SliderFloat("cam 2 far", &orthoF, -400.0f, 400.0f);
			ImGui::End();

			FrameBuffer::Unbind();
			glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);

			gBuffer.Bind();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			gBuffer.LightingPass(frameBuffer, shadowCam, shadowMap.GetDepthTexture().GetID());
			//gBuffer.ShadowMap();
			
			//Renderer::BlitTexture(frameBuffer, {});

			//PostProcessing::ShadowCastGLSL(camera, gBuffer);

			//copy zbuffer data from gbuffer to default framebuffer
			//so debug drawing isn't always in front of geometry of screen.
			Renderer::BlitFrameBuffer(gBuffer.GetID(), frameBuffer.GetID(), GL_DEPTH_BUFFER_BIT);

			renderer.SetAlphaBlending(true);
			ImGui::Begin("meme");
			ImGui::Checkbox("draw bvh", &drawBvh);
			ImGui::End();

			frameBuffer.Bind();
			//Renderer::Clear(GL_COLOR_BUFFER_BIT);
			//Renderer::ClearColor(1, 1, 1, 1);
		
			if (drawBvh) bvh.Draw(camera, renderer);
			//debugFrame.Unbind();

			glm::vec3 vScale, vTrans, skew;
			glm::quat vRot;
			glm::vec4 persp;
			glm::decompose(shadowCam.GetProjectionMatrix() * shadowCam.GetViewMatrix(), vScale, vRot, vTrans, skew, persp);


			float H = SCREENWIDTH / 8;
			float V = SCREENHEIGHT / 8;
			float znear = shadowCam.GetNearPlaneDist();
			float zfar = shadowCam.GetFarPlaneDist();

			glm::mat4 scale2 = glm::scale(glm::mat4(1.0f), { H  , V  , (zfar + znear) });
			shadowCam.SetViewVector(-lightDir);
			glm::mat4 vmi = glm::inverse(shadowCam.GetViewMatrix());
			glm::mat4 scale2x = glm::scale(glm::mat4(1.0f), { 2,2,2 });
			glm::mat4 Rmat = glm::rotate(glm::mat4(1.0f), 3.1415f / 2.0f, { 0, 0, 1 });
			glm::mat4 pmi = glm::inverse(shadowCam.GetProjectionMatrix()) * scale2x * glm::inverse(Rmat);


			//ImGui::InputFloat3("shadowCam move", &shadowCam.GetPosition()[0]);
			//ImGui::SliderFloat3("shadowCam pos", &shadowCam.GetPosition()[0], -100, 100);
			//lol += 0.01f;
			//debugFrame.Bind();
			glLineWidth(7.0f);
			renderer.DrawCube(camera, vmi * pmi, glm::vec4(1, 0, 0, 1));
			renderer.DrawCube(camera, vmi, glm::vec4(0, 1, 0, 1));
			//renderer.DrawCube(camera, translate(mat4(1.0f), vec3(shadowCam.GetPosition())), vec4(0, 0, 1, 1));
			renderer.DrawLine(glm::mat4(1.0f), camera, glm::vec3(0), 5.0f * LightManager::GetDirectionalLight());
			//renderer.DrawLine(vmi * pmi * Rmat, camera, vec3(0), vec3(-10, 0, 0));
			glLineWidth(1.0f);
			frameBuffer.Unbind();
			

			std::pair<GLuint, std::string> bufferTargets[5] =
			{
				{ shadowMap.GetTexture().GetID(), "shadow"},
				{ gBuffer.GetPositionID(), "Position"},
				{ gBuffer.GetNormalID(), "Normal"},
				{ gBuffer.GetZBufferTexID(), "Zbuffer" },
				{ frameBuffer.GetTexture().GetID(), "frame buffer" }
			};

			int my_image_width = SCREENWIDTH / 5;
			int my_image_height = SCREENHEIGHT / 5;

			ImGui::Begin("G-buffer Textures");
			ImGui::Columns(4, "mixed", false);
			ImGui::Separator();
			for (auto& [texID, name] : bufferTargets)
			{
				float thumbW = static_cast<float>(my_image_width);
				float thumbH = static_cast<float>(my_image_height);
				ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texID)),
					ImVec2(thumbW, thumbH), ImVec2(0, 1), ImVec2(1, 0));
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texID)),
						ImVec2(thumbW * 4, thumbH * 4), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::EndTooltip();
				}

				ImGui::Text(name.c_str());
				ImGui::Text("ID = %i", texID);
				ImGui::Text("Size = %i x %i", my_image_width, my_image_height);

				ImGui::NextColumn();
			}
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			ImVec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint64_t textureID = frameBuffer.GetTexture().GetID();
			ImGui::Image(reinterpret_cast<void*>(textureID), viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
			ImGui::PopStyleVar();

			ImGui::Begin("meme");
			ImGui::SliderFloat3("camera pos", &camera.GetPosition()[0], -100, 100);
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			UserInterface::Draw();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
			
			Renderer::SwapBuffers(window);
			renderer.CalcFrameTime(deltaTime);
			frame++;


		}
	}

	ShaderManager::Destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}




//#include "test1.h"
////#include "Application.h"
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <fmt/format.h>
////#include "precomp.h"
//#include <vendor/imgui/imgui.h>
//#include <vendor/imgui/imgui_impl_glfw.h>
//#include <vendor/imgui/imgui_impl_opengl3.h> 
////#include <vendor/imgui/extensions/imGuIZMOquat.h> 
//#include <entt/entt.hpp>
//#include "Geometry/BVH/BVH.h"
////#include <glm/glm.hpp>
//int SCREENWIDTH = 1024;
//int SCREENHEIGHT = 1024;
//
//int main(void)
//{
//	if (!glfwInit()) return -1;
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
//	glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE
//
//	//glfwWindowHint(GLFW_FULLSCREEN, GL_TRUE);
//	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//	// glfwGetPrimaryMonitor() ;
//
//	/* Create a windowed mode window and its OpenGL context */
//	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Test window", nullptr, nullptr);
//
//	if (!window)
//	{
//		glfwTerminate();
//		throw std::exception("ERROR: Could not create GLFW window!");
//		return -1;
//	}
//
//	glfwMakeContextCurrent(window);
//
//	if (glewInit() != GLEW_OK) fmt::print("ERROR! CAN'T INIT GLEW\n");
//
//	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
//
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO(); (void)io;
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//	//BVH bvh;
//	// Setup Dear ImGui style
//	ImGui::StyleColorsDark();
//	//ImGui::StyleColorsClassic();
//	const char* glsl_version = "#version 130";
//	// Setup Platform/Renderer bindings
//	ImGui_ImplGlfw_InitForOpenGL(window, true);
//	ImGui_ImplOpenGL3_Init(glsl_version);
//	BVH bvh;
//	while (!glfwWindowShouldClose(window))
//	{
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//		ImGui::ShowDemoWindow();
//		glClearColor(1, 1, 0.5f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//			glfwSetWindowShouldClose(window, true);
//
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//
//	glfwDestroyWindow(window);
//	glfwTerminate();
//	return 0;
//}
