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
	float aa = ((1.0f * 10.0f - 1.0f / 0.1f + (2.0f - 0.5f * 4.0f) - 0.000f));
	float xx = 1.0f / aa;
	std::cout << glGetString(GL_VERSION) << std::endl;
	GLint  max_tex_size;
	GLint  max_uniform_locations;
	GLCall(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size));
	GLCall(glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &max_uniform_locations));

	std::cout << "--- GPU Data ---\n";
	std::cout << "max tex size: " << max_tex_size << "\n";
	std::cout << "max uniform locations: " << max_uniform_locations << "\n";
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
		spyro.name = "spyro";
		EntityManager::AddEntity(spyro);
		// spyro.getMesh(0).MakeWireFrame();

		//Model duck = Model("res/meshes/bvhtest/rubber-ducky.obj", aiProcess_Triangulate);
		//duck.name = "duck";
		//duck.SetShader("framebuffers");
		//EntityManager::AddEntity(duck);

		//Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiProcess_Triangulate);
		//artisans.SetShader("basic");
		//artisans.name = "artisans";
		//EntityManager::AddEntity(artisans);

		//TriangleBuffer::GetTriangleBuffer();

		std::vector<glm::vec3>& triBuffer = spyro.getMesh(0).positionVertices;
		/*for (auto& v : spyro.getMesh(0).positionVertices)
		{
			 v  = (spyro.model * glm::vec4(v, 1.0f));
		}*/

		//unsigned int triCount = triBuffer.size();
		//
		//unsigned int triTex;
		//GLCall(glGenTextures(1, &triTex));
		//GLCall(glBindTexture(GL_TEXTURE_1D, triTex));
		//GLCall(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, triCount, 0,
		//	GL_RGB, GL_FLOAT, &triBuffer[0]));

		// GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 1, 0,
		// 	GL_RGB, GL_FLOAT, &triverts[0]));

		GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP));
		GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP));
		GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		//GLCall(glBindTexture(GL_TEXTURE_2D, triTex));

		//artisans.getMesh(0).MakeWireFrame();
		//Model nanosuit("res/meshes/nanosuit/nanosuit.obj", (aiPostProcessSteps)(aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace));
		//nanosuit.name = "nanosuit";
		//EntityManager::AddEntity(nanosuit);
		//nanosuit.SetShader("normalmapshader");

		BVH bvh;
		bvh.BuildBVH();
		bvh.CreateBVHTextures();

		std::cout << "bvh size: " << sizeof(bvh.m_pool[0]) * bvh.m_poolPtr << "\n";
		//
		constexpr float half = 0.5f;
		float triVerts[] =
		{
			-half, -half, 1, 0, 0,
			half, -half, 0, 1, 0,
			half,	half, 0, 0, 1
		};

		// screen quad VAO
		unsigned int triVAO, triVBO;
		glGenVertexArrays(1, &triVAO);
		glGenBuffers(1, &triVBO);
		glBindVertexArray(triVAO);
		glBindBuffer(GL_ARRAY_BUFFER, triVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triVerts), &triVerts, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		// screen quad VAO
		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),
			&quadVertices, GL_STATIC_DRAW); //TODO: static?
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		float lineverts[] = { 0, 0, 1, 1 };

		unsigned int lineVao, lineVBO;
		glGenVertexArrays(1, &lineVao);
		glGenBuffers(1, &lineVBO);
		glBindVertexArray(lineVao);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineverts), &lineverts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

		Shader& postProcessShader = ShaderManager::GetShader("framebuffers_screen");

		postProcessShader.Bind();
		postProcessShader.SetInt("screenTexture", 0);
		postProcessShader.SetInt("u_triangleTexture", 1);
		postProcessShader.SetInt("u_indexTexture", 2); //bvh data
		postProcessShader.SetInt("u_minTexture", 3);
		postProcessShader.SetInt("u_maxTexture", 4);
		postProcessShader.SetInt("u_triangleIdxTexture", 5);

		Shader& lineshader = ShaderManager::GetShader("lineshader");
		Shader& boneshader = ShaderManager::GetShader("bones");

		// framebuffer configuration
		// -------------------------
		////////////////////////////

		FrameBuffer framebuffer;

		static bool output = true;
		// create a color attachment texture
		unsigned int textureColorbuffer;
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		RenderBufferObject rbo;
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Texture back("res/textures/uvtest.png");

		const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;

		Camera camera(glm::vec3(0, 3, 16), 70, aspect, 0.1f, 200.0f);
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

		glEnable(GL_DEPTH_TEST);

		std::vector<float>  frametimes;

		glm::vec3 lightpos(1.0f, 2.0f, 3.0f);
		glm::mat4 pp = glm::translate(glm::mat4(1.0f), lightpos);
		glm::vec4 s = pp[3];
		glm::vec4  override_color(0.3f, 0.5, 0.1, 0.5f);

		//auto& singledotshader = ShaderManager::GetShader("singledotshader");
		//
		// Render loop
		//

		LightManager::AddLight({ 0,  5, 0 }, { 1,0,0 });
		LightManager::AddLight({ -2,  3, 0 }, { 0,1,0 });
		LightManager::AddLight({ 2,  3, 0 }, { 0,0,1 });

		const glm::vec3 upWorld(0, 1, 0);
		const glm::vec3 rightWorld(1, 0, 0);
		const glm::vec3 forwardWorld(0, 0, -1);


		double prevFrameTime = glfwGetTime();
		glm::vec4 particleColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		// enable depth testing (is disabled for rendering screen-space quad)



		int currentBbox = 0;
		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);	double currentFrameTime = glfwGetTime();
			float deltaTime = currentFrameTime - prevFrameTime;



			//ImGui::ShowDemoWindow(&showDemo);



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

			if (ImGui::gizmo3D("##Dir1", light))
				lightDir = glm::vec3(-light.x, -light.y, -light.z);


			//ImGui::SliderFloat3("directional light", &lightDir[0], -1, 1);




			float& ambientLight = LightManager::GetAmbientLight();
			ImGui::SliderFloat("ambient", &ambientLight, 0, 1);

			//cube.Update(deltaTime);
			//plane.Update(deltaTime);
			//spyro.Update(deltaTime);
			//duck.Update(deltaTime);
			//nanosuit.Update(deltaTime);
			//artisans.Update(deltaTime);

		//	cube.Draw(camera);
			//plane.Draw(camera);
			spyro.Draw(camera);
			//duck.Draw(camera);
			//artisans.Draw(camera);
			//spyro.getMesh(0).DrawWireFrame(camera, spyro.model);

			//artisans.Draw(camera);
			// artisans.getMesh(0).DrawWireFrame(camera, artisans.model);

			// nanosuit.GetShader().Bind();
			// nanosuit.GetShader().setVec3("lightPos", LightManager::GetLight(0).get_position());
			// nanosuit.GetShader().setVec3("viewPos", camera.GetPosition());
			// nanosuit.Draw(camera);

			//set bvh node
			ImGui::SliderInt("bbox idx", &currentBbox, 0, bvh.m_poolPtr - 1);

			GLCall(glDisable(GL_DEPTH_TEST));
			bvh.DrawSingleAABB(camera, currentBbox);
			int leftfirst = bvh.m_pool[currentBbox].m_leftFirst;
			int rightFirst = leftfirst + 1;

			bvh.DrawSingleAABB(camera, leftfirst);
			bvh.DrawSingleAABB(camera, rightFirst);

			GLCall(glEnable(GL_DEPTH_TEST));

			// draw AABB instanced
			//bvh.Draw(camera);

			double MouseX, MouseY;
			glfwGetCursorPos(window, &MouseX, &MouseY);

			const Ray ray = camera.RayFromMouse(MouseX, MouseY);
			bvh.TraceRay(ray);

			LightManager::debug_render(camera);

			prevFrameTime = currentFrameTime;
			glDisable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			const glm::vec3 camPos = camera.GetPosition();
			const float nearPlane = camera.GetNearPlaneDist();
			const glm::vec3 camForward = (1.1f + nearPlane) * camera.GetForwardVector();
			const glm::vec3 camUp = camera.GetUpVector();

			postProcessShader.Bind();
			postProcessShader.setVec3("u_cameraPos", camPos);
			postProcessShader.SetFloat("u_screenWidth", float(SCREENWIDTH));
			postProcessShader.SetFloat("u_screenHeight", float(SCREENHEIGHT));
			postProcessShader.SetUniformMat4f("u_inv_projMatrix", glm::inverse(camera.GetProjectionMatrix()));
			postProcessShader.SetUniformMat4f("u_inv_viewMatrix", glm::inverse(camera.GetViewMatrix()));

			AABB& aabb = spyro.getMesh(0).m_aabb;

			//draw final quad
			glBindVertexArray(quadVAO);
			GLCall(glActiveTexture(GL_TEXTURE0 + 0));
			GLCall(glBindTexture(GL_TEXTURE_2D, textureColorbuffer));

			bvh.GetTriangleTexture().Bind(1);
			bvh.GetAABBNodesTexture().Bind(2);
			bvh.GetMinTexture().Bind(3);
			bvh.GetMaxTexture().Bind(4);
			bvh.GetTriangleIndexTexture().Bind(5);
			
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

			userInterface.Draw();

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currentFrameTime);
			frametimes.push_back(static_cast<float>(diffms));
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


