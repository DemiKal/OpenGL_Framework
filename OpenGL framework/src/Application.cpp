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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE); //GL_FALSE GL_TRUE
	glfwSwapInterval(0);
	//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

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
	glfwSwapInterval(0);
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
	{
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
		spyro.SetShader("basic");
		spyro.name = "Spyro";
		EntityManager::AddEntity(spyro);
		// spyro.getMesh(0).MakeWireFrame();

		//Model duck = Model("res/meshes/bvhtest/rubber-ducky.obj", aiProcess_Triangulate);
		//duck.name = "duck";
		//duck.SetShader("framebuffers");
		//EntityManager::AddEntity(duck);

		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		artisans.SetShader("basic");
		artisans.name = "artisans";
		EntityManager::AddEntity(artisans);

		BVH bvh;
		bvh.BuildBVH(2);
		bvh.CreateBVHTextures();

		std::cout << "bvh size: " << sizeof(bvh.m_pool[0]) * bvh.m_poolPtr / 1024 << "kb \n";

		Shader& postProcessShader = ShaderManager::GetShader("framebuffers_screen");

		postProcessShader.Bind();
		postProcessShader.SetInt("screenTexture", 0);
		postProcessShader.SetInt("u_triangleTexture", 1);
		postProcessShader.SetInt("u_indexTexture", 2); //bvh data
		postProcessShader.SetInt("u_minTexture", 3);
		postProcessShader.SetInt("u_maxTexture", 4);
		postProcessShader.SetInt("u_triangleIdxTexture", 5);
		postProcessShader.SetInt("u_depthBuffer", 6);
		postProcessShader.SetInt("u_positionBuffer", 7);

		Shader& lineshader = ShaderManager::GetShader("lineshader");
		Shader& boneshader = ShaderManager::GetShader("bones");

		// framebuffer configuration
		// -------------------------
		////////////////////////////

		FrameBuffer framebuffer;

		// create a color attachment texture
		Texture2D textureColorBuffer(
			GL_RGB32F,
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

		const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;

		Camera camera(glm::vec3(0, 3, 16), 70, aspect, 0.1f, 400.0f);
		Camera cam2(glm::vec3(-10, 3, 0), 70, aspect, 0.1f, 200.0f);
		cam2.RotateYlocal(glm::radians(-90.0f));

		Camera::SetMainCamera(&camera);
		Camera::SetCamera2(&cam2);

		FrameBuffer posBuffer;
		Texture2D positionTexture(
			GL_RGB32F,
			SCREENWIDTH,
			SCREENHEIGHT,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE);

		Texture2D zBufferTexPosition(
			GL_DEPTH_COMPONENT32,
			SCREENWIDTH,
			SCREENHEIGHT,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture.GetID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zBufferTexPosition.GetID(), 0);

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

 
		static bool u_shadowCast = false;

		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			double currentFrameTime = glfwGetTime();
			float deltaTime = currentFrameTime - prevFrameTime;

#pragma region input
			userInterface.Update();
			InputManager::Update(camera);


			const float average = (float)std::accumulate(frametimes.begin(), frametimes.end(), 0.0) / frametimes.size();
			const float avgfps = 1000.0f * float(1.0f / average);

			ImGui::Text("ms %f", avgfps);

			if (frametimes.size() >= 30) frametimes.clear();
#pragma endregion input

			framebuffer.Bind();
			// make sure we clear the framebuffer's content
			glClearColor(0.12f, 0.2f, 0.13f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::vec3& lightDir = LightManager::GetDirectionalLight();
			//glm::vec3 light(-lightDir);
			vgm::Vec3 light(-lightDir.x, -lightDir.y, -lightDir.z);
			// get/setLigth are helper funcs that you have ideally 
			//defined to manage your global/member objs
			//if (ImGui::gizmo3D("##Dir1", light /*, size,  mode */)  setLight(-light);
			//	// or explicitly

			lightDir.x = cos(0.5 * currentFrameTime);
			lightDir.z = sin(0.5 * currentFrameTime);
			//lightDir = glm::normalize(lightDir);
			if (ImGui::gizmo3D("##Dir1", light))
				lightDir = glm::vec3(-light.x, -light.y, -light.z);


			float& ambientLight = LightManager::GetAmbientLight();
			ImGui::SliderFloat("ambient", &ambientLight, 0, 1);

			//cube.Update(deltaTime);
			//plane.Update(deltaTime);
			spyro.Update(deltaTime);
			//duck.Update(deltaTime);
			//nanosuit.Update(deltaTime);
			//artisans.Update(deltaTime);

			artisans.Draw(camera);
			spyro.Draw(camera);


			posBuffer.Bind();
			glClearColor(0.12f, 0.2f, 0.13f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Shader& posShader = ShaderManager::GetShader("position");
			artisans.Draw(camera, posShader);
			spyro.Draw(camera, posShader);




			// nanosuit.GetShader().Bind();
			// nanosuit.GetShader().setVec3("lightPos", LightManager::GetLight(0).get_position());
			// nanosuit.GetShader().setVec3("viewPos", camera.GetPosition());
			// nanosuit.Draw(camera);

			//set bvh node
			//ImGui::SliderInt("bbox idx", &currentBbox, 0, bvh.m_poolPtr - 1);

			//GLCall(glDisable(GL_DEPTH_TEST));
			//bvh.DrawSingleAABB(camera, currentBbox);
			//int leftfirst = bvh.m_pool[currentBbox].m_leftFirst;
			//int rightFirst = leftfirst + 1;

			if (ImGui::RadioButton("Draw bvh", &drawBvh)) drawBvh = !drawBvh;

			if (drawBvh) bvh.Draw(camera);


			// draw AABB instanced
			//bvh.Draw(camera);
			static float angle = 45;
			ImGui::SliderAngle("angle slider", &angle);

			LightManager::debug_render(camera);

			prevFrameTime = currentFrameTime;
			glDisable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind fb

			const glm::vec3 camPos = camera.GetPosition();
			const float nearPlane = camera.GetNearPlaneDist();
			const glm::vec3 camForward = (1.1f + nearPlane) * camera.GetForwardVector();
			const glm::vec3 camUp = camera.GetUpVector();

			ImGui::Checkbox("Shadow cast", &u_shadowCast);



			postProcessShader.Bind();
			postProcessShader.setVec3("u_cameraPos", camPos);
			postProcessShader.SetFloat("u_screenWidth", float(SCREENWIDTH));
			postProcessShader.SetFloat("u_screenHeight", float(SCREENHEIGHT));
			postProcessShader.SetUniformMat4f("u_inv_projMatrix", glm::inverse(camera.GetProjectionMatrix()));
			postProcessShader.SetUniformMat4f("u_inv_viewMatrix", glm::inverse(camera.GetViewMatrix()));

			postProcessShader.SetFloat("u_near_plane", camera.GetNearPlaneDist());
			postProcessShader.SetFloat("u_far_plane", camera.GetFarPlaneDist());

		 
			postProcessShader.setVec3("u_lightDir", lightDir);
			postProcessShader.SetInt("u_shadowCast", u_shadowCast);
			static bool useZ = false;

			if (u_shadowCast) {
				ImGui::Checkbox("Zbuffer for position?", &useZ);
				postProcessShader.SetInt("u_useZbuffer", useZ);

				if (useZ)
				{
					const char* listbox_items[] = { "Logarithmic","Linear 1", "Linear 2" };
					static int depthReconstrMode = 0;
					ImGui::ListBox("Depth Reconstruction Mode\n(single select)", &depthReconstrMode, listbox_items, IM_ARRAYSIZE(listbox_items), 3);
					postProcessShader.SetInt("u_DepthReconstruction", depthReconstrMode);

				}
			}
			//static bool integrTri = false;
			//ImGui::Checkbox("Use integrated triangles?", &integrTri);
			//postProcessShader.SetInt("u_integrTri", integrTri);
			//draw final quad
			screenQuad.Bind();

			GLCall(glActiveTexture(GL_TEXTURE0 + 0));
			GLCall(glBindTexture(GL_TEXTURE_2D, textureColorbufferID));

			bvh.GetTriangleTexture().Bind(1);
			bvh.GetAABBNodesTexture().Bind(2);
			bvh.GetMinTexture().Bind(3);
			bvh.GetMaxTexture().Bind(4);
			bvh.GetTriangleIndexTexture().Bind(5);

			GLCall(glActiveTexture(GL_TEXTURE0 + 6));
			GLCall(glBindTexture(GL_TEXTURE_2D, zBufferTextureID));

			GLCall(glActiveTexture(GL_TEXTURE0 + 7));
			GLCall(glBindTexture(GL_TEXTURE_2D, positionTexture.GetID()));

			screenQuad.Draw();
			screenQuad.UnBind();
			static float angleee = 0;
			angleee += 0.01f;

			static auto bcolor = glm::vec4(1, 0.5f, 0, 1);
			ImGui::ColorEdit4("bcolor", glm::value_ptr(bcolor));
			renderer.DrawCube(camera, glm::rotate(glm::mat4(1.0f), angleee, { 0,1,0 })
				* glm::scale(glm::mat4(1.0f), { cos(angleee), 0.5 + 0.5 * sin(angleee),-cos(angleee) }), bcolor);


			userInterface.Draw();

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currentFrameTime);
			frametimes.push_back(static_cast<float>(diffms));
		}


		ShaderManager::Destroy();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);

		glfwTerminate();
		return 0;
	}
}


