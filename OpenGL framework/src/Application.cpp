#include "precomp.h"
#include "Application.h"
#include "Rendering/Buffer/FrameBuffer.h"
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
#include "misc/UserInterface.h"
#include "Rendering/ScreenQuad.h"
#include "Rendering/PostProcessing.h"
#include "Geometry/BVH/BVHNode.h"

//#define BUNNY
//#define DRAGON

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
	bool pauseLight = false;
	bool alphaBlend = false;
	bool vsync = true;

	if (glewInit() != GLEW_OK) fmt::print("ERROR!\n");

	HardwareQuery::Query();
	{
		Renderer renderer;
		renderer.SetAlphaBlending(alphaBlend);
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

#ifdef BUNNY
		Model bunny("res/meshes/bunny.obj", aiProcess_Triangulate);
		bunny.SetShader("Gbuffer_basic");
		bunny.m_name = "Spyro";
		EntityManager::AddEntity(bunny);
#endif
#ifdef DRAGON
		Model dragon("res/meshes/dragon.obj", aiProcess_Triangulate);
		dragon.SetShader("Gbuffer_basic");
		dragon.m_name = "erato";
		EntityManager::AddEntity(dragon);
#endif

		BVH bvh;
		bvh.BuildBVH(renderer);

		FrameBuffer frameBuffer;

		// configure g-buffer framebuffer
		// ------------------------------
		Gbuffer gBuffer;
		gBuffer.SetShader("deferred_shading");
		gBuffer.BindShader();
		//deferredShading.Bind();

		const float aspect = static_cast<float>(SCREENWIDTH) / static_cast<float>(SCREENHEIGHT);
		const auto originalCamPos = glm::vec3(0, 3, 16);
		Camera camera(originalCamPos, 70, aspect, 0.1f, 400.0f);
		Camera cam2(glm::vec3(-10, 3, 0), 70, aspect, 0.1f, 200.0f);
		cam2.RotateLocalY(glm::radians(-90.0f));
		const glm::mat4 cam1Mat = camera.GetViewMatrix();
		const glm::mat4 cam2Mat = cam2.GetViewMatrix();
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

		//ScreenQuad screenQuad;

		LightManager::AddLight({ 0, 5, 0 }, { 1, 0, 0 });
		LightManager::AddLight({ -2, 3, 0 }, { 0, 1, 0 });
		LightManager::AddLight({ 2, 3, 0 }, { 0, 0, 1 });

		double prevFrameTime = glfwGetTime();

		bool drawBvh = false;


		double totalTime = 0;
		float rotation = 0;
		int fbw, fbh, w, h;
		glfwGetFramebufferSize(window, &fbw, &fbh);
		glfwGetWindowSize(window, &w, &h);
		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			Renderer::ClearColor(1, 1, 1, 1);
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Renderer::EnableDepth();
			renderer.SetDepthFunc(GL_LEQUAL);
			renderer.SetCullingMode(GL_BACK);
			const double currentFrameTime = glfwGetTime();
			float deltaTime = static_cast<float>(currentFrameTime - prevFrameTime);
			renderer.SetAlphaBlending(false);

#pragma region input
			userInterface.Update();
			InputManager::Update(camera, deltaTime);

			ImGui::Checkbox("Alpha Blend", &alphaBlend);
			ImGui::Checkbox("VSync", &vsync);
			ImGui::Checkbox("Pause Light", &pauseLight);



			renderer.SetAlphaBlending(alphaBlend);
			renderer.SetVSync(vsync);

			const float average = (float)std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
			const float avgFPS = 1000.0f * static_cast<float>(1.0f / average);

			ImGui::Text("FPS: %f, %f ms", avgFPS, average);

			if (frameTimes.size() >= 100) frameTimes.clear();

			glm::vec3& lightDir = LightManager::GetDirectionalLight();
			if (!pauseLight)
			{
				rotation += 1.f * deltaTime;
				lightDir.x = sinf(rotation);
				lightDir.z = cosf(rotation);
			}
			vgm::Vec3 light(-lightDir.x, -lightDir.y, -lightDir.z);

			if (ImGui::gizmo3D("##Dir1", light)) lightDir = glm::vec3(-light.x, -light.y, -light.z);
			float& ambientLight = LightManager::GetAmbientLight();
			ImGui::SliderFloat("ambient", &ambientLight, 0, 1);
#pragma endregion input

			//update meshes
			artisans.Update(deltaTime);

#ifdef BUNNY//update meshes
			bunny.Update(deltaTime);
#endif

			spyro.Update(deltaTime);
			Renderer::EnableDepth();
			gBuffer.Bind();
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//draw meshes
			artisans.Draw(camera);
#ifdef BUNNY
			bunny.Draw(camera);
#endif

#ifdef DRAGON
			dragon.Draw(camera);
#endif
			spyro.Draw(camera);
			gBuffer.LightingPass(frameBuffer);

			Renderer::DrawScreenQuad();
			FrameBuffer::Unbind();
			Renderer::BlitTexture(frameBuffer, {});

			//PostProcessing::ShadowCastGLSL(camera, gBuffer);

			Renderer::BlitFrameBuffer(gBuffer.GetID(), 0, GL_DEPTH_BUFFER_BIT);

			renderer.SetAlphaBlending(true);
			ImGui::Checkbox("draw bvh", &drawBvh);
			if (drawBvh) bvh.Draw(camera, renderer);

			std::pair<GLuint, std::string> bufferTargets[4] = {
				{ gBuffer.GetAlbedoSpecID(), "Albedo"},
				{ gBuffer.GetPositionID(), "Position"},
				{ gBuffer.GetNormalID(), "Normal"},
				{ gBuffer.GetZBufferTexID(), "Zbuffer" } };

			int my_image_width = SCREENWIDTH / 5;
			int my_image_height = SCREENHEIGHT / 5;

			ImGui::Begin("G-buffer Textures");
			ImGui::Columns(4, "mixed", false);
			ImGui::Separator();
			for (auto [texID, name] : bufferTargets)
			{
				ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texID)),
					ImVec2(static_cast<float>(my_image_width),
							static_cast<float>(my_image_height)),
					ImVec2(0, 1), ImVec2(1, 0));

				ImGui::Text(name.c_str());
				ImGui::Text("ID = %i", texID);
				ImGui::Text("Size = %i x %i", my_image_width, my_image_height);

				ImGui::NextColumn();
			}

			ImGui::End();

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


