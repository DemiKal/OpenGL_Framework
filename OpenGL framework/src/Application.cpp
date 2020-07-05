#include "precomp.h"
#include "Application.h"
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

#ifndef _DEBUG
#define BUNNY
#define DRAGON
#endif
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

		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		artisans.SetShader("Gbuffer_basic");
		artisans.m_name = "artisans";
		EntityManager::AddEntity(artisans);

#ifdef BUNNY
		Model bunny("res/meshes/erato/erato.obj", aiProcess_Triangulate);
		bunny.SetShader("Gbuffer_basic");
		bunny.m_name = "Spyro";
		EntityManager::AddEntity(bunny);
#endif
#ifdef DRAGON
		Model dragon("res/meshes/dragon.obj", aiProcess_Triangulate);
		dragon.SetShader("Gbuffer_basic_no_tex");
		dragon.m_name = "erato";
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
		glm::vec3 dirLightPos = glm::vec3(0, 90, -250);

		Camera cam2(dirLightPos, 70, aspect, 0.1f, 700.0f);
		//cam2.RotateLocalY(180);
		//cam2.RotateLocalX(25);
		float orthoW = 20.0f;
		float orthoH = 40.0f;

		cam2.SetOrthographic(orthoW, orthoH, -40, 40);

		//	cam2.RotateLocalX(glm::radians( 90.0f));

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


		//ScreenQuad screenQuad;

		LightManager::AddLight({ 0, 5, 0 }, { 1, 0, 0 });
		LightManager::AddLight({ -2, 3, 0 }, { 0, 1, 0 });
		LightManager::AddLight({ 2, 3, 0 }, { 0, 0, 1 });

		renderer.m_prevFrameTime = (float)glfwGetTime();

		bool drawBvh = false;

		float nearLight = 0.1f;
		float farLight = 100.0f;
		float camLR = 10.0f;
		float camUD = 10.0f;
		glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearLight, farLight);
		//glm::mat4 lightProj = glm::perspective(70.0f, aspect, nearLight, farLight);

		cam2.GetPosition() = { 0,20,-10 };

		int shadowWidth = 1024;
		int shadowHeight = 1024;

		//double totalTime = 0;
		//float rotation = 0;
		int fbw, fbh, w, h;
		glfwGetFramebufferSize(window, &fbw, &fbh);
		glfwGetWindowSize(window, &w, &h);
		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
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
#ifdef BUNNY//update meshes
			bunny.Update(deltaTime);
#endif

			Renderer::EnableDepth();
			gBuffer.Bind();
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
			//camera.SetViewVector(cam2.GetForwardVector());
			//camera.GetPosition() = cam2.PositionRead();

			//draw meshes
			artisans.Draw(camera);
			spyro.Draw(camera);

			glm::vec3& lightDir = LightManager::GetDirectionalLight();

			ImGui::SliderFloat("near Light", &nearLight, 0, farLight);
			ImGui::SliderFloat("far Light", &farLight, nearLight, 500);

			//LightManager::DebugRender(camera);

		//	lightProj = glm::project(-20.0f, 20.0f, -20.0f, 20.0f, nearLight, farLight);

			glm::mat4 lightView = glm::lookAt(dirLightPos, dirLightPos - lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
			//glm::mat4 rot = glm::eulerAngleYXZ(
			//	glm::radians(lightDir.y), glm::radians(lightDir.x), glm::radians(lightDir.z));
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), dirLightPos);



			glm::mat4 lightMatrix = lightProj * trans * lightView;
			auto xx = camera.GetPosition();
			auto xx2 = camera.GetViewMatrix();
			auto xx3 = xx2 * vec4(0, 0, 0, 1);

			//cam2.SetViewVector();

			//cam2.LookAt(vec3(0));
			//camera.GetPosition() = dirLightPos;

			//glViewport(0, 0, 1024, 1024);
			shadowMap.Bind();
			renderer.EnableDepth();
			//renderer.SetCullingMode(GL_FRONT);
			Renderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Renderer::ClearColor(1, 1, 1, 1);
			artisans.Draw(cam2, agnosticShader);
			spyro.Draw(cam2, agnosticShader);
			FrameBuffer::Unbind();
			//renderer.SetCullingMode(GL_BACK);

#ifdef BUNNY
			bunny.Draw(camera);
#endif

#ifdef DRAGON
			dragon.Draw(camera);
#endif
			//spyro.Draw(camera);


			glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);

			gBuffer.Bind();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			gBuffer.LightingPass(frameBuffer, cam2, shadowMap.GetDepthTexture().GetID());
			//gBuffer.ShadowMap();

			Renderer::DrawScreenQuad();
			FrameBuffer::Unbind();
			Renderer::BlitTexture(frameBuffer, {});

			//PostProcessing::ShadowCastGLSL(camera, gBuffer);

			Renderer::BlitFrameBuffer(gBuffer.GetID(), 0, GL_DEPTH_BUFFER_BIT);

			renderer.SetAlphaBlending(true);
			ImGui::Checkbox("draw bvh", &drawBvh);
			if (drawBvh) bvh.Draw(camera, renderer);
			//glm::mat4 transformation; // your transformation matrix.
			//glm::vec3 scale;
			//glm::quat rotation;
			//glm::vec3 translation;
			//glm::vec3 skew;
			//glm::vec4 perspective;
			//glm::decompose(transformation, scale, rotation, translation, skew, perspective);

			//ImGui::SliderAngle("view dir light", &)

			//auto t =   glm::translate(mat4(1.0f), cam2.GetPosition());
			auto rot = glm::eulerAngleYXZ(
				(lightDir.y),
				(lightDir.x),
				(lightDir.z));

			auto rot90 = glm::rotate(mat4(1.0f), glm::radians(90.0f), { 1,0,0 });
			auto mat3 = cam2.GetViewMatrix();
			//	renderer.DrawCube(camera, lightView, vec4(0.8f, 0.3f, 0.1f, 1));

			float length = farLight - nearLight;
			auto scl = scale(mat4(1.0f), vec3(camLR * 2, length, camUD * 2));
			mat4 trans2 = translate(mat4(1.0f), normalize(lightDir) * (nearLight + length / 2.0f));
			mat4 t = translate(mat4(1.0f), cam2.GetPosition());

			mat4 projBox = trans * rot * scl;


			glm::vec3 vScale, vTrans, skew;
			glm::quat vRot;
			glm::vec4 persp;
			glm::decompose(cam2.GetProjectionMatrix() * cam2.GetViewMatrix(), vScale, vRot, vTrans, skew, persp);

			//renderer.DrawCube(camera, inverse(cam2.GetViewMatrix()), vec4(0.1f, 1.0f, 0.2f, 1));
			//renderer.DrawCube(camera, t, vec4(1.0f, 0.0f, 0.1f, 1));

			//static vec3 lineA = vec3(0, 0, 0);
			//static vec3 lineB = vec3(0, 0, -5);
			//ImGui::InputFloat3("Draw line A", &lineA[0]);
			//ImGui::InputFloat3("Draw line B", &lineB[0]);
			//auto proj = cam2.GetProjectionMatrix();

			float H = SCREENWIDTH / 8;
			float V = SCREENHEIGHT / 8;
			float znear = cam2.GetNearPlaneDist();
			float zfar = cam2.GetFarPlaneDist();

			mat4 scale2 = scale(mat4(1.0f), { H  , V  , (zfar + znear) });
			cam2.SetViewVector(lightDir);
			mat4 vmi = inverse(cam2.GetViewMatrix());
			mat4 scale2x = scale(mat4(1.0f), { 2,2,2 });
			mat4 Rmat = rotate(mat4(1.0f), 3.1415f/2.0f, { 0, 0, 1 });
			mat4 pmi =    inverse(cam2.GetProjectionMatrix()) * scale2x  * inverse(Rmat);

			static float lol = 0;
			lol += 0.01f;
			//	mat4 tmat = glm::translate(mat4(1.0f), { 0, 0,  -(znear + zfar) / 2.0f });

			//ImGui::InputFloat3("cam2 move", &cam2.GetPosition()[0]);
			ImGui::SliderFloat3("cam2 pos", &cam2.GetPosition()[0], -100, 100);
			//lol += 0.01f;
			glLineWidth(7.0f);
			renderer.DrawCube(camera, vmi * pmi, vec4(1, 0, 0, 1));
			renderer.DrawCube(camera, vmi, vec4(0, 1, 0, 1));
			renderer.DrawCube(camera, translate(mat4(1.0f), vec3(cam2.GetPosition())), vec4(0, 0, 1, 1));

			renderer.DrawLine(mat4(1.0f), camera, vec3(0), vec3(10, 0, 0));
			renderer.DrawLine(vmi * pmi * Rmat, camera, vec3(0), vec3(-10, 0, 0));


			glLineWidth(1.0f);


			std::pair<GLuint, std::string> bufferTargets[4] = {
				{ shadowMap.GetTexture().GetID(), "Albedo"},
				{ gBuffer.GetPositionID(), "Position"},
				{ gBuffer.GetNormalID(), "Normal"},
				{ gBuffer.GetZBufferTexID(), "Zbuffer" } };

			int my_image_width = SCREENWIDTH / 6;
			int my_image_height = SCREENHEIGHT / 6;

			ImGui::Begin("G-buffer Textures");
			ImGui::Columns(4, "mixed", false);
			ImGui::Separator();
			for (auto& [texID, name] : bufferTargets)
			{
				float thumbW = static_cast<float>(my_image_width);
				float thumbH = static_cast<float>(my_image_height);
				ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texID)),
					ImVec2(thumbW, thumbH), ImVec2(0, 1), ImVec2(1, 0));
				if (ImGui::IsItemHovered() && name == "Albedo")
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

			ImGui::SliderFloat3("camera pos", &camera.GetPosition()[0], -100, 100);

			UserInterface::Draw();
			Renderer::SwapBuffers(window);
			renderer.CalcFrameTime(deltaTime);

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


