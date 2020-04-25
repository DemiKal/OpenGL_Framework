#include "precomp.h"
#include "Application.h"
#include "Rendering/Buffer/RenderBufferObject.h"
#include "Rendering/Buffer/FrameBuffer.h"
#include "GameObject/Components/Texture2D.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Renderer.h"
#include "GameObject/Camera.h"
#include "Light/LightManager.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/mesh.h"
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

	if (!window)
	{
		glfwTerminate();
		throw std::exception("ERROR: Could not create GLFW window!");
		return -1;
	}

	InputManager::SetWindow(window);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) std::cout << "ERROR!" << std::endl;


	std::cout << glGetString(GL_VERSION) << std::endl;
	GLint  max_tex_size;
	GLint  max_uniform_locations;
	GLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size));
	GLCall(glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &max_uniform_locations));

	std::cout << "--- GPU Data ---\n";
	std::cout << "max tex size: " << max_tex_size << "\n";
	std::cout << "max uniform locations: " << max_uniform_locations / 1024 << "kb \n";
	std::cout << "----------------\n";
	std::vector<int> nums{ 3, 4, 2, 8, 15, 267 };

	auto print = [](const int& n) { std::cout << " " << n; };

	std::cout << "before:";
	//std::for_each(std::execution::par, nums.cbegin(), nums.cend(), print);
	std::cout << '\n';

	std::for_each(std::execution::par, std::begin(nums), std::end(nums),
		[&](int i)
		{
			std::cout << i << "\n";
			//nums.fetch_add(1, std::memory_order_relaxed);
		});


	struct position
	{
		position(const float x, const  float y) : pos(x, y) {}
		glm::vec2 pos;
	};

	struct velocity
	{
		velocity(const float x, const  float y) : vel(x, y) {}
		glm::vec2 vel;
	};

	struct aaaaaaaaaaaa
	{
		int i;
	};
	{
		entt::registry registry;

		//entt::registry registry;
		std::uint64_t dt = 16;

		for (auto i = 0; i < 10; ++i)
		{
			auto entity = registry.create();
			registry.assign<entt::tag<"enemy"_hs>>(entity);

			registry.assign<position>(entity, i * 1.f, i * 1.f);
			if (i % 2 == 0) registry.assign<velocity>(entity, i * .1f, i * .1f);
		}
		auto view = registry.view<position, velocity>();
		for (auto& entity : view) {
			// const auto [ cpos , cvel] = view.get<const position,velocity>(entity);
			//  cvel = { -150, -10 };
			//position& vel = view.get<position>(entity);
			//const position& cpos = view.get<const position>(entity);
			//const velocity& cvel = view.get<const velocity>(entity);
			//std::tuple<position&, const velocity&> tup = view.get<position, const velocity>(entity);
			//std::tuple<const position&, const velocity&> ctup = view.get<const position, const velocity>(entity);
		}
		//auto view2 = registry.view<position>();
		//for (auto& entity :view2 )
		//{
		//	auto& pos = view2.get<position>(entity);
		//	std::cout << "pos: { " << pos.pos.x << ", " << pos.pos.y << " }" << "\n";
		//}
		//auto view3 = registry.view<velocity>();
		//for (auto& entity : view3)
		//{
		//	auto& vel = view3.get<velocity>(entity);
		//	std::cout << "vel: { " << vel.vel.x << ", " << vel.vel.y << " }" << "\n";
		//}
		//auto view = registry.view<position, velocity>();
		//for (auto entity : view)
		//{
		//	auto& vel = view.get<velocity>(entity);
		//}

			//auto view = registry.view<position, velocity>();

		Renderer renderer;
		renderer.SetAlphaBlending(true);

		//init before any model
		ShaderManager::Init();
		UserInterface userInterface;

		//Model cube = Model::CreateCube();
		//cube.name = "cube";
		////
		//EntityManager::AddEntity(cube);
		//cube.SetShader("framebuffers");
		//cube.getMesh(0).addTexture(Texture2D("res/textures/marble.jpg", "texture_diffuse"));

		//Model plane = Model::CreatePlane();
		//plane.name = "plane";
		////plane.SetShader("framebuffers");
		//plane.SetShader("plane");
		//plane.getMesh(0).addTexture(Texture2D("res/textures/brickwall.jpg", "texture_diffuse"));
		//EntityManager::AddEntity(plane);

		Model wireCube = Model::CreateCubeWireframe();
		wireCube.name = "WireCube";
		EntityManager::AddEntity(wireCube);
		wireCube.SetShader("AABB_instanced");

		Model spyro("res/meshes/Spyro/Spyro.obj", aiProcess_Triangulate);
		spyro.SetShader("Gbuffer_basic");
		spyro.name = "Spyro";
		EntityManager::AddEntity(spyro);
		// spyro.getMesh(0).MakeWireFrame();

		//Model duck = Model("res/meshes/bvhtest/rubber-ducky.obj", aiProcess_Triangulate);
		//duck.name = "duck";
		//duck.SetShader("framebuffers");
		//EntityManager::AddEntity(duck);

		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		artisans.SetShader("Gbuffer_basic");
		artisans.name = "artisans";
		EntityManager::AddEntity(artisans);

		//artisans.getMesh(0).MakeWireFrame();
		//Model nanosuit("res/meshes/nanosuit/nanosuit.obj", (aiPostProcessSteps)(aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace));
		//nanosuit.name = "nanosuit";
		//EntityManager::AddEntity(nanosuit);
		//nanosuit.SetShader("normalmapshader");

		BVH bvh;
		bvh.BuildBVH();
		bvh.CreateBVHTextures();

		std::cout << "bvh size: " << sizeof(bvh.m_pool[0]) * bvh.m_poolPtr / 1024 << "kb \n";
		//

		Shader& postProcessShader = ShaderManager::GetShader("framebuffers_screen");

		postProcessShader.Bind();
		postProcessShader.SetInt("screenTexture", 0);
		postProcessShader.SetInt("u_triangleTexture", 1);
		postProcessShader.SetInt("u_indexTexture", 2); //bvh data
		postProcessShader.SetInt("u_minTexture", 3);
		postProcessShader.SetInt("u_maxTexture", 4);
		postProcessShader.SetInt("u_triangleIdxTexture", 5);
		postProcessShader.SetInt("u_depthBuffer", 6);

		Shader& lineshader = ShaderManager::GetShader("lineshader");
		Shader& boneshader = ShaderManager::GetShader("bones");

		// framebuffer configuration
		// -------------------------
		////////////////////////////

		FrameBuffer framebuffer;

		// create a color attachment texture
		Texture2D textureColorBuffer(
			GL_RGB,
			SCREENWIDTH,
			SCREENHEIGHT,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE);

		unsigned int textureColorbufferID = textureColorBuffer.GetID();

		Texture2D zBufferTex(
			GL_DEPTH_COMPONENT24,
			SCREENWIDTH,
			SCREENHEIGHT,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT);

		unsigned int zBufferTextureID = zBufferTex.GetID();

		//bind color and depth to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbufferID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBufferTextureID, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// configure g-buffer framebuffer
// ------------------------------
		unsigned int gBuffer;
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		unsigned int gPosition, gNormal, gAlbedoSpec;
		// position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		// normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		// color + specular color buffer
		glGenTextures(1, &gAlbedoSpec);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		Shader deferredShading = ShaderManager::GetShader("deferred_shading");

		deferredShading.Bind();
		deferredShading.SetInt("gPosition", 0);
		deferredShading.SetInt("gNormal", 1);
		deferredShading.SetInt("gAlbedoSpec", 2);





		//  create and attach depth buffer (renderbuffer)
		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREENWIDTH, SCREENHEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;

		Camera camera(glm::vec3(0, 3, 16), 70, aspect, 0.1f, 400.0f);
		Camera cam2(glm::vec3(-10, 3, 0), 70, aspect, 0.1f, 200.0f);
		cam2.RotateYlocal(glm::radians(-90.0f));

		Camera::SetMainCamera(&camera);
		Camera::SetCamera2(&cam2);
		//Camera* cam = Camera::GetMain(); //??
		//* cam->Position()   += glm::vec3(2, 0, 3);



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

		std::vector<float>  frametimes;

		glm::vec3 lightpos(1.0f, 2.0f, 3.0f);
		glm::mat4 pp = glm::translate(glm::mat4(1.0f), lightpos);
		glm::vec4 s = pp[3];
		glm::vec4  override_color(0.3f, 0.5, 0.1, 0.5f);

		ScreenQuad screenQuad;

		LightManager::AddLight({ 0,  5, 0 }, { 1, 0,0 });
		LightManager::AddLight({ -2,  3, 0 }, { 0,1,0 });
		LightManager::AddLight({ 2,  3, 0 }, { 0,0,1 });

		const glm::vec3 upWorld(0, 1, 0);
		const glm::vec3 rightWorld(1, 0, 0);
		const glm::vec3 forwardWorld(0, 0, -1);

		double prevFrameTime = glfwGetTime();
		glm::vec4 particleColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		bool drawBvh = false;
		int currentBbox = 0;

		float smoothStart = 0;
		float smoothEnd = 1;
		double totalTime = 0;
		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			double currentFrameTime = glfwGetTime();
			float deltaTime = currentFrameTime - prevFrameTime;
			renderer.SetAlphaBlending(false);

#pragma region input
			userInterface.Update();
			InputManager::Update(camera);


			const float average = (float)std::accumulate(frametimes.begin(), frametimes.end(), 0.0) / frametimes.size();
			const float avgfps = 1000.0f * float(1.0f / average);

			ImGui::Text("ms %f", avgfps);

			if (frametimes.size() >= 30) frametimes.clear();
#pragma endregion input

			//framebuffer.Bind();
			// make sure we clear the framebuffer's content
			//glClearColor(0.12f, 0.2f, 0.13f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::vec3& lightDir = LightManager::GetDirectionalLight();
			lightDir.x = sinf(totalTime);
			lightDir.z = cosf(totalTime);
			// lightDir.y =   cosf(totalTime) ;

			//glm::vec3 light(-lightDir);
			vgm::Vec3 light(-lightDir.x, -lightDir.y, -lightDir.z);
			// get/setLigth are helper funcs that you have ideally 
			//defined to manage your global/member objs
			//if (ImGui::gizmo3D("##Dir1", light /*, size,  mode */)  setLight(-light);
			//	// or explicitly

			if (ImGui::gizmo3D("##Dir1", light))
				lightDir = glm::vec3(-light.x, -light.y, -light.z);
			float& ambientLight = LightManager::GetAmbientLight();
			ImGui::SliderFloat("ambient", &ambientLight, 0, 1);

			static float fogDistance = 20.0f;
			ImGui::SliderFloat("fogDistance", &fogDistance, 0, 100);

			//cube.Update(deltaTime);
			//plane.Update(deltaTime);
			spyro.Update(deltaTime);
			//duck.Update(deltaTime);
			//nanosuit.Update(deltaTime);
			artisans.Update(deltaTime);

			//artisans.Draw(camera);
			//glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 1. geometry pass: render scene's geometry/color data into gbuffer
			// -----------------------------------------------------------------
			glEnable(GL_DEPTH);
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//	Shader& gbufferShaderGeometry = ShaderManager::GetShader("Gbuffer_basic");
				//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			//	gbufferShaderGeometry.Bind();
			//	gbufferShaderGeometry.SetUniformMat4f("u_projection", camera.GetProjectionMatrix());
			//	gbufferShaderGeometry.SetUniformMat4f("u_view", camera.GetViewMatrix());
			//	gbufferShaderGeometry.SetUniformMat4f("u_model", spyro.model);
			//	
			artisans.Draw(camera);
			spyro.Draw(camera);
			renderer.SetAlphaBlending(true);
			//glDisable(GL_DEPTH_TEST);
			


			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// nanosuit.GetShader().Bind();
			// nanosuit.GetShader().setVec3("lightPos", LightManager::GetLight(0).get_position());
			// nanosuit.GetShader().setVec3("viewPos", camera.GetPosition());
			// nanosuit.Draw(camera);

			//set bvh node
			//ImGui::SliderInt("bbox idx", &currentBbox, 0, bvh.m_poolPtr - 1);





			// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
			// -----------------------------------------------------------------------------------------------------------------------
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			deferredShading.Bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPosition);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
			// send light relevant uniforms
			deferredShading.setVec3("u_globalLightDir", lightDir);
			deferredShading.SetFloat("u_ambientLight", ambientLight);
			deferredShading.SetFloat("u_fogDistance", fogDistance);
			deferredShading.setVec3("u_viewPos", camera.GetPosition());
			const char* listbox_items[] = { "Regular Shading", "Albedo", "Normals", "Position", "Specular" };
			static int displayMode = 0;
			ImGui::ListBox("listbox\n(single select)", &displayMode, listbox_items, IM_ARRAYSIZE(listbox_items), 5);

			deferredShading.SetInt("u_displayMode", displayMode);

			screenQuad.Bind();
			screenQuad.Draw();
			//deferredShading.setVec3("viewPos", camera.Position);
			// finally render quad
			 glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
			 glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			 glBlitFramebuffer(0, 0, SCREENWIDTH, SCREENHEIGHT, 0, 0, SCREENWIDTH, SCREENHEIGHT,
			 	GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			bvh.Draw(camera);
			//renderQuad();
			// draw AABB instanced
			//bvh.Draw(camera);
			static float angle = 45;
			ImGui::SliderAngle("angle slider", &angle);

			//LightManager::debug_render(camera);
			if (ImGui::RadioButton("Draw bvh", &drawBvh)) drawBvh = !drawBvh;
			//glBindFramebuffer(GL_FRAMEBUFFER ,0);
			//framebuffer.Bind();
		

			//post process ray tracer
			//glDisable(GL_DEPTH_TEST);
			//
			//glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind fb
			//
			//const glm::vec3 camPos = camera.GetPosition();
			//const float nearPlane = camera.GetNearPlaneDist();
			//const glm::vec3 camForward = (1.1f + nearPlane) * camera.GetForwardVector();
			//const glm::vec3 camUp = camera.GetUpVector();
			//
			//postProcessShader.Bind();
			//postProcessShader.setVec3("u_cameraPos", camPos);
			//postProcessShader.SetFloat("u_screenWidth", float(SCREENWIDTH));
			//postProcessShader.SetFloat("u_screenHeight", float(SCREENHEIGHT));
			//postProcessShader.SetUniformMat4f("u_inv_projMatrix", glm::inverse(camera.GetProjectionMatrix()));
			//postProcessShader.SetUniformMat4f("u_inv_viewMatrix", glm::inverse(camera.GetViewMatrix()));
			//
			//postProcessShader.SetFloat("u_near_plane", camera.GetNearPlaneDist());
			//postProcessShader.SetFloat("u_far_plane", camera.GetFarPlaneDist());
			//
			//ImGui::SliderFloat("smoothstep start", &smoothStart, 0, .9f);
			//ImGui::SliderFloat("smoothstep end", &smoothEnd, 0.9f, 2);
			//postProcessShader.SetFloat("u_smoothStepStart", smoothStart);
			//postProcessShader.SetFloat("u_smoothStepEnd", smoothEnd);
			//
			////draw final quad
			//screenQuad.Bind();
			//
			//GLCall(glActiveTexture(GL_TEXTURE0 + 0));
			//GLCall(glBindTexture(GL_TEXTURE_2D, textureColorbufferID));
			//
			//bvh.GetTriangleTexture().Bind(1);
			//bvh.GetAABBNodesTexture().Bind(2);
			//bvh.GetMinTexture().Bind(3);
			//bvh.GetMaxTexture().Bind(4);
			//bvh.GetTriangleIndexTexture().Bind(5);
			//
			//GLCall(glActiveTexture(GL_TEXTURE0 + 6));
			//GLCall(glBindTexture(GL_TEXTURE_2D, zBufferTextureID));
			//
			//screenQuad.Draw();
			//screenQuad.UnBind();
			////
			//// ray trace postprocessing
			////






			static float angleee = 0;
			angleee += 0.01f;

			static auto bcolor = glm::vec4(1, 0.5f, 0, 1);
			ImGui::ColorEdit4("bcolor", glm::value_ptr(bcolor));
			renderer.DrawCube(camera, glm::rotate(glm::mat4(1.0f), angleee, { 0,1,0 })
				* glm::scale(glm::mat4(1.0f), { cos(angleee), 0.5 + 0.5 * sin(angleee),-cos(angleee) }), bcolor);


			userInterface.Draw();

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			prevFrameTime = currentFrameTime;
			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currentFrameTime);
			frametimes.push_back(static_cast<float>(diffms));
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


