#include "precomp.h"
#include "Application.h"
#include "Rendering/Buffer/FrameBuffer.h"
#include "GameObject/Components/Texture2D.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/Buffer/Gbuffer.h"
#include "GameObject/Camera.h"
#include "Light/LightManager.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/Model.h"
#include "misc/InputManager.h"	
#include "Geometry/BVH/BVH.h"
#include "Geometry/BVH/BVHNode.h"
#include "Geometry/Ray.h"
#include "misc/UserInterface.h"
#include <exception>
#include <Rendering/ScreenQuad.h>
#include <algorithm>
#include <execution>

#include "Rendering/PostProcessing.h"


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

	if (glewInit() != GLEW_OK) std::cout << "ERROR!" << std::endl;
	HardwareQuery::Query();

	{
		Renderer renderer;

		static bool alphaBlend = false;

		renderer.SetAlphaBlending(alphaBlend);

		static bool vsync = true;

		renderer.SetVSync(vsync);


		//init before any model
		ShaderManager::Init();
		UserInterface userInterface;
		
		//Model wireCube = Model::CreateCubeWireframe();
		//wireCube.m_name = "WireCube";
		//EntityManager::AddEntity(wireCube);
		//wireCube.SetShader("AABB_instanced");

		Model spyro("res/meshes/Spyro/Spyro.obj", aiProcess_Triangulate);
		spyro.SetShader("Gbuffer_basic");
		spyro.m_name = "Spyro";
		EntityManager::AddEntity(spyro);

#ifndef _DEBUG
		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		artisans.SetShader("Gbuffer_basic");
		artisans.m_name = "artisans";
		EntityManager::AddEntity(artisans);
#endif
		

		BVH bvh;
		bvh.BuildBVH(renderer);





		FrameBuffer framebuffer;


		// configure g-buffer framebuffer
		// ------------------------------

		Gbuffer gBuffer;
		 
		Shader shadow_casting = ShaderManager::GetShader("shadow_cast");
		shadow_casting.Bind();
		shadow_casting.SetInt("gPosition", 0);
		shadow_casting.SetInt("gNormal", 1);
		shadow_casting.SetInt("gAlbedoSpec", 2);
		shadow_casting.Unbind();
		
		Shader deferredShading = ShaderManager::GetShader("deferred_shading");

		deferredShading.Bind();
		deferredShading.SetInt("gPosition", 0);
		deferredShading.SetInt("gNormal", 1);
		deferredShading.SetInt("gAlbedoSpec", 2);
 

		
		const float aspect = static_cast<float>(SCREENWIDTH) / static_cast<float>(SCREENHEIGHT);

		Camera camera(glm::vec3(0, 3, 16), 70, aspect, 0.1f, 400.0f);
		Camera cam2(glm::vec3(-10, 3, 0), 70, aspect, 0.1f, 200.0f);
		cam2.RotateLocalY(glm::radians(-90.0f));

		Camera::SetMainCamera(&camera);
		Camera::SetCamera2(&cam2);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		const char* glsl_version = "#version 130";
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		std::vector<float>  frameTimes;

		ScreenQuad screenQuad;

		LightManager::AddLight({ 0,  5, 0 }, { 1, 0,0 });
		LightManager::AddLight({ -2,  3, 0 }, { 0,1,0 });
		LightManager::AddLight({ 2,  3, 0 }, { 0,0,1 });

		double prevFrameTime = glfwGetTime();

		bool drawBvh = false;


		double totalTime = 0;

		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			Renderer::ClearColor(1,1,1, 1);
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::EnableDepth();
			Renderer::SetDepthFunc(GL_LEQUAL);
			const double currentFrameTime = glfwGetTime();
			float deltaTime = currentFrameTime - prevFrameTime;
			renderer.SetAlphaBlending(false);

#pragma region input
			userInterface.Update();
			InputManager::Update(camera,   deltaTime);

			ImGui::Checkbox("Alpha Blend", &alphaBlend);
			ImGui::Checkbox("VSync", &vsync);

			renderer.SetAlphaBlending(alphaBlend); 
			renderer.SetVSync(vsync);

			const float average = (float)std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
			const float avgfps = 1000.0f * static_cast<float>(1.0f / average);

			ImGui::Text("ms %f", avgfps);

			if (frameTimes.size() >= 30) frameTimes.clear();
#pragma endregion input

			glm::vec3& lightDir = LightManager::GetDirectionalLight();
			lightDir.x = sinf(totalTime);
			lightDir.z = cosf(totalTime);

			vgm::Vec3 light(-lightDir.x, -lightDir.y, -lightDir.z);

			if (ImGui::gizmo3D("##Dir1", light)) lightDir = glm::vec3(-light.x, -light.y, -light.z);
			float& ambientLight = LightManager::GetAmbientLight();
			ImGui::SliderFloat("ambient", &ambientLight, 0, 1);

#ifndef _DEBUG
			artisans.Update(deltaTime);
#endif

			spyro.Update(deltaTime);

			// 1. geometry pass: render scene's geometry/color data into gbuffer
			// -----------------------------------------------------------------
			Renderer::EnableDepth();
			//glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			gBuffer.Bind();
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifndef _DEBUG
			 artisans.Draw(camera);
#endif
			
			spyro.Draw(camera);

			spyro.m_position += glm::vec3(2, 1, 0);
			spyro.UpdateModelMatrix();
			spyro.Draw(camera);
			spyro.m_position -= glm::vec3(2, 1, 0);
			spyro.UpdateModelMatrix();
			
			renderer.SetAlphaBlending(true);

			framebuffer.Bind();	//make sure we write to this framebuffer

			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			deferredShading.Bind();
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gBuffer.GetPositionID());
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, gBuffer.GetNormalID());
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, gBuffer.GetAlbedoSpecID());

			// send light relevant uniforms
			deferredShading.SetVec3f("u_globalLightDir", lightDir);
			deferredShading.SetFloat("u_ambientLight", ambientLight);
			//;deferredShading.SetVec3f("u_viewPos", camera.GetPosition());

			const char* listbox_items[] = { "Regular Shading", "Albedo", "Normals", "Position", "Specular" };
			static int displayMode = 0;
			ImGui::ListBox("listbox\n(single select)", &displayMode, listbox_items, IM_ARRAYSIZE(listbox_items), 5);

			deferredShading.SetInt("u_displayMode", displayMode);

			//framebuffer.Bind();
			screenQuad.Bind();
			ScreenQuad::Draw();	//Draw to custom frame buffer
			
			ImGui::Checkbox("draw bvh", &drawBvh);

			//draw from deferred to framebuffer to standard framebuffer
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.GetTexture().GetID(), 0); //crash!

			//Shader& singleTex = ShaderManager::GetShader("framebuffer_screen");
			//singleTex.Bind();
			//framebuffer.GetTexture().Bind();
			//ScreenQuad::Draw();
			PostProcessing::ShadowCastGLSL(camera, gBuffer);

			Renderer::BlitFrameBuffer(gBuffer.GetID(), 0, GL_DEPTH_BUFFER_BIT);

			//Renderer::SetDepthFunc(GL_LEQUAL);


			///
			/// Draw extra widgets, gizmos, debug info, and more below
			//
			static float angle = 0; 	angle += 0.01f;
			renderer.DrawCube(camera, glm::rotate(glm::mat4(1.0f), angle, { 0,1,0 })
				* glm::scale(glm::mat4(1.0f), { cos(angle), 0.5 + 0.5 * sin(angle), -cos(angle) }), {0,1,1,1});
		 
			if (drawBvh) bvh.Draw (camera, renderer);
			UserInterface::Draw();


			Renderer::SwapBuffers(window);

			prevFrameTime = currentFrameTime;
			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currentFrameTime);
			frameTimes.push_back(static_cast<float>(diffms));
			totalTime += deltaTime;
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


