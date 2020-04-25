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
		wireCube.m_name = "WireCube";
		EntityManager::AddEntity(wireCube);
		wireCube.SetShader("AABB_instanced");

		Model spyro("res/meshes/Spyro/Spyro.obj", aiProcess_Triangulate);
		spyro.SetShader("Gbuffer_basic");
		spyro.m_name = "Spyro";
		EntityManager::AddEntity(spyro);
		// spyro.getMesh(0).MakeWireFrame();

		//Model duck = Model("res/meshes/bvhtest/rubber-ducky.obj", aiProcess_Triangulate);
		//duck.name = "duck";
		//duck.SetShader("framebuffers");
		//EntityManager::AddEntity(duck);

		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		artisans.SetShader("Gbuffer_basic");
		artisans.m_name = "artisans";
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

		Gbuffer G_buffer;
		unsigned int gbuffer = G_buffer.GetID();

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
		cam2.RotateLocalY(glm::radians(-90.0f));

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

		

		double prevFrameTime = glfwGetTime();
		glm::vec4 particleColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		bool drawBvh = false;
		int currentBbox = 0;


		double totalTime = 0;
		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			renderer.ClearColor(0, 0, 0, 1);
			renderer.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glClear();
			renderer.Enable(GL_DEPTH_TEST);
			renderer.SetDepthFunc(GL_LESS);
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
			renderer.EnableDepth();
			//glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			G_buffer.Bind();
			renderer.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			artisans.Draw(camera);
			spyro.Draw(camera);
			renderer.SetAlphaBlending(true);
			
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
			glBindTexture(GL_TEXTURE_2D, G_buffer.GetPositionID());
			glActiveTexture(GL_TEXTURE0+1);
			glBindTexture(GL_TEXTURE_2D, G_buffer.GetNormalID());
			glActiveTexture(GL_TEXTURE0+2);
			glBindTexture(GL_TEXTURE_2D, G_buffer.GetAlbedoSpecID());

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
		
			renderer.BlitFrameBuffer(G_buffer.GetID(), 0, GL_DEPTH_BUFFER_BIT);
			

			//LightManager::debug_render(camera);
			if (ImGui::RadioButton("Draw bvh", &drawBvh)) 
				drawBvh = !drawBvh;
			
			if(drawBvh) bvh.Draw(camera);
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


