#include "precomp.h"
#include "Application.h"

int main(void)
{
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	InputManager::SetWindow(window);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) std::cout << "ERROR!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	{
		Renderer renderer;
		renderer.SetAlphaBlending(true);
		ShaderManager::Init(); //init before any model

		//Model obj = Model();
		//obj.loadModel("res/meshes/nanosuit/nanosuit.obj", aiProcess_Triangulate);
		//obj.SetShader("normalmapshader");



		
		Model cube = Model::CreateCube();
		EntityManager::AddEntity(cube);
		cube.SetShader("framebuffers");
		cube.getMesh(0).addTexture(Texture2D("res/textures/marble.jpg", "texture_diffuse"));

		EntityManager::GetEntities()[0]->m_position = { 0,5,0 };

		Model plane = Model::CreatePlane();
		EntityManager::AddEntity(plane);

		plane.SetShader("plane");
		plane.getMesh(0).addTexture(Texture2D("res/textures/brickwall.jpg", "texture_diffuse"));

		Model wireCube = Model::CreateCubeWireframe();
		EntityManager::AddEntity(wireCube);

		wireCube.SetShader("singlecolor");

		Model spyro("res/meshes/Spyro/Spyro.obj", aiPostProcessSteps::aiProcess_Triangulate);
		spyro.SetShader("basic");
		EntityManager::AddEntity(spyro);

		Model artisans("res/meshes/Spyro/Artisans Hub/Artisans Hub.obj", aiPostProcessSteps::aiProcess_Triangulate);
		artisans.SetShader("basic");
		EntityManager::AddEntity(artisans);

		Model nanosuit("res/meshes/nanosuit/nanosuit.obj", aiPostProcessSteps::aiProcess_CalcTangentSpace);
		nanosuit.SetShader("normalmapshader");

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_DRAW);
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

		GPUShader& postProcessShader = ShaderManager::GetShader("framebuffers_screen");

		postProcessShader.Bind();
		postProcessShader.SetInt("screenTexture", 0);

		GPUShader& lineshader = ShaderManager::GetShader("lineshader");
		GPUShader& boneshader = ShaderManager::GetShader("bones");

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

		Texture back("res/textures/uvtest.png");

		const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;

		Camera camera(glm::vec3(0, 3, 16), 70, aspect, 0.1f, 200.0f);
		Camera cam2(glm::vec3(-10, 3, 0), 70, aspect, 0.1f, 200.0f);
		cam2.RotateYlocal(glm::radians(-90.0f));

		Camera::SetMainCamera(&cam2);
		//Camera* cam = Camera::GetMain(); //??
		//* cam->Position()   += glm::vec3(2, 0, 3);
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		glEnable(GL_DEPTH_TEST);

		std::vector<float>  frametimes;

		glm::vec3 lightpos(1.0f, 2.0f, 3.0f);
		glm::mat4 pp = glm::translate(glm::mat4(1.0f), lightpos);
		glm::vec4 s = pp[3];
		glm::vec4  override_color(0.3f, 0.5, 0.1, 0.5f);

		auto& singledotshader = ShaderManager::GetShader("singledotshader");
		//
		// Render loop
		//

		LightManager::AddLight({ 0,  5, 0 }, { 1,0,0 });
		LightManager::AddLight({ -2,  3, 0 }, { 0,1,0 });
		LightManager::AddLight({ 2,  3, 0 }, { 0,0,1 });

		const glm::vec3 upWorld(0, 1, 0);
		const glm::vec3 rightWorld(1, 0, 0);
		const glm::vec3 forwardWorld(0, 0, -1);

		//auto tt = glm::rotate(plane.model, glm::radians(45.0f+90.0f), { 0,0,1 });
		//plane.getMesh(0).m_aabb.RecalcBounds(tt);
		//wireCube.model = plane.getMesh(0).m_aabb.GetTransform();
		//plane.model *= tt;
		double prevFrameTime = glfwGetTime();

		//Game Loop
		while (!glfwWindowShouldClose(window))
		{
			double currentFrameTime = glfwGetTime();
			float deltaTime = currentFrameTime - prevFrameTime;

			ImGui_ImplGlfwGL3_NewFrame();

#pragma region input

			InputManager::Update(camera);

			const float average = (float)std::accumulate(frametimes.begin(), frametimes.end(), 0.0) / frametimes.size();
			const float avgfps = 1000.0f * float(1.0f / average);

			ImGui::Text("ms %f", avgfps);

			if (frametimes.size() >= 30) frametimes.clear();
#pragma endregion input

			framebuffer.Bind();
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			// make sure we clear the framebuffer's content
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cube.Update(deltaTime);
			plane.Update(deltaTime);
			spyro.Update(deltaTime);

			cube.Draw(camera);
			plane.Draw(camera);
			spyro.Draw(camera);
			artisans.Draw(camera);

			nanosuit.GetShader().Bind();
			nanosuit.GetShader().setVec3("lightPos", LightManager::GetLight(0).get_position());
			nanosuit.GetShader().setVec3("viewPos", camera.GetPosition());


			nanosuit.Draw(camera);

			LightManager::debug_render(camera);

			glDisable(GL_DEPTH_TEST);

			prevFrameTime = currentFrameTime;

			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			ImGui::ColorEdit4("clear color", static_cast<float*>(&override_color[0]));
			ImGui::SliderFloat3("Position", &plane.m_position[0], -100.0f, 100.0f);
			ImGui::SliderFloat3("Rotation", &plane.m_rotation[0], 0, 360);
			ImGui::SliderFloat3("Scale", &plane.m_scale[0], 0, 10);

			postProcessShader.Bind();

			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());


			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currentFrameTime);
			frametimes.push_back(static_cast<float>(diffms));
		}
	}

	ShaderManager::Destroy();
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}


