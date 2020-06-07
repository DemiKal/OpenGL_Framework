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
//#include <CL/opencl.h>

class vertex
{
	float x, y, z;

public:
	vertex(float x, float y, float z) : x(x), y(y), z(z) {};

	//explicit vertex(const vertex& o) : x(o.x), y(o.y), z(o.z) { std::cout << "Copy!\n"; };
	//vertex(vertex& o) : x(o.x), y(o.y), z(o.z) { std::cout << "Copy!\n"; };

	vertex(const vertex&) = delete;
	vertex& operator=(const vertex&) = delete;
	vertex(vertex&& v) noexcept
		: x(v.x), y(v.y), z(v.z)
	{
		std::cout << "Move 1!\n";
	}

	vertex& operator=(vertex&& v) noexcept
	{
		std::cout << "Move 2!\n";
		return v;
	}

};

int main(void)
{
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE); //GL_FALSE GL_TRUE

	//glfwWindowHint(GLFW_FULLSCREEN, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// glfwGetPrimaryMonitor() ;

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", nullptr, NULL);

	std::vector< vertex> vertices;
	vertices.reserve(3);
	vertices.emplace_back( 1, 2, 3);
	vertices.emplace_back( 4, 5, 6);
	vertices.emplace_back( 7, 8, 9);

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

		Model spyro("res/meshes/Spyro/Spyro.obj", aiProcess_Triangulate);
		spyro.SetShader("Gbuffer_basic");
		spyro.m_name = "Spyro";
		EntityManager::AddEntity(spyro);

		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		artisans.SetShader("Gbuffer_basic");
		artisans.m_name = "artisans";
		EntityManager::AddEntity(artisans);

		const double startTime = glfwGetTime();

		BVH bvh;
#ifndef _DEBUG
		bvh.BuildBVH();
		bvh.CreateBVHTextures();
#endif
		const double endTime = glfwGetTime();
		double time = endTime - startTime;

		std::cout << "bvh size: " << sizeof(bvh.m_pool[0]) * bvh.m_poolPtr / 1024 << "kb \n";
		std::cout << "seconds: " << time << "\n";


		FrameBuffer framebuffer;


		// configure g-buffer framebuffer
		// ------------------------------

		Gbuffer G_buffer;

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
			const float avgfps = 1000.0f * float(1.0f / average);

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

			spyro.Update(deltaTime);

			// 1. geometry pass: render scene's geometry/color data into gbuffer
			// -----------------------------------------------------------------
			Renderer::EnableDepth();
			//glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			G_buffer.Bind();
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			artisans.Draw(camera);
			spyro.Draw(camera);
			renderer.SetAlphaBlending(true);

			framebuffer.Bind();	//make sure we write to this framebuffer

			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			deferredShading.Bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, G_buffer.GetPositionID());
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, G_buffer.GetNormalID());
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, G_buffer.GetAlbedoSpecID());

			// send light relevant uniforms
			deferredShading.SetVec3f("u_globalLightDir", lightDir);
			deferredShading.SetFloat("u_ambientLight", ambientLight);
			deferredShading.SetVec3f("u_viewPos", camera.GetPosition());

			const char* listbox_items[] = { "Regular Shading", "Albedo", "Normals", "Position", "Specular" };
			static int displayMode = 0;
			ImGui::ListBox("listbox\n(single select)", &displayMode, listbox_items, IM_ARRAYSIZE(listbox_items), 5);

			deferredShading.SetInt("u_displayMode", displayMode);

			//framebuffer.Bind();
			screenQuad.Bind();
			ScreenQuad::Draw();	//Draw to custom frame buffer

			//draw from deferred to framebuffer to standard framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.GetTexture().GetID(), 0);

			Shader& singleTex = ShaderManager::GetShader("framebuffer_screen");
			singleTex.Bind();
			framebuffer.GetTexture().Bind();
			ScreenQuad::Draw();

			Renderer::BlitFrameBuffer(G_buffer.GetID(), 0, GL_DEPTH_BUFFER_BIT);
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


