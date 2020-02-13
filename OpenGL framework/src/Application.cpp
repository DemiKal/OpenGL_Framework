#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32 
#include "precomp.h"

void   GetArmatureVertices(std::shared_ptr<Model::Armature> arma, std::vector<glm::vec3>& verts)
{
	glm::vec3 pos = arma->mat * glm::vec4(0, 0, 0, 1);
	verts.emplace_back(pos);

	for (auto& child : arma->children) GetArmatureVertices(child, verts);

}

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


	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) std::cout << "ERROR!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	{
		Renderer renderer;
		renderer.SetAlphaBlending(true);
		ShaderManager::Init(); //init before any model

		Model obj = Model();
		obj.loadModel("res/meshes/Crash Bandicoot/Spike/SpikeHero.obj", aiProcess_Triangulate);
		obj.SetShader("basic");

		Model cube = Model::CreateCube();
		cube.SetShader("framebuffers");
		cube.meshes[0].m_textures.emplace_back(Texture2D("res/textures/marble.jpg", "texture_diffuse"));

		Model plane = Model::CreatePlane();
		plane.SetShader("plane");
		plane.meshes[0].m_textures.emplace_back(Texture2D("res/textures/brickwall.jpg", "texture_diffuse"));

		//
		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
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

		float lineverts[] = { 0,0, 1,1 };

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
		//unsigned int fb;
		//glGenFramebuffers(1, &fb);
		//glBindFramebuffer(GL_FRAMEBUFFER, fb);


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



		///
		///  GO DOWNWARDS FROM HERE FOR FBO
		///
		///


		Texture back("res/textures/uvtest.png");

		const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;

		Camera camera(glm::vec3(0, 3, 8), 70, aspect, 0.1f, 200.0f);

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();
		float rot = 0;


		//game loop
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);

		double mouseXold = 0, mouseYold = 0;
		double mouseXnew = 0, mouseYnew = 0;
		//std::clock_t start;
		//double duration;
		//Camera cam;
		Transform t;
		t.SetPos({ 0, 0, 0 });
		t.SetRot({ 0, 0, 0 });
		t.SetScale({ 1, 1,1 });
		int i = 0;
		std::vector<float>  frametimes;

		glm::vec3 lightpos(1.0f, 2.0f, 3.0f);
		glm::mat4 pp = glm::translate(glm::mat4(1.0f), lightpos);
		glm::vec4 s = pp[3];
		glm::vec4  override_color(0.3f, 0.5, 0.1, 0.5f);

		//
		// Render loop
		//
		//obj.model = glm::rotate(obj.model, glm::radians(-90.f), glm::vec3(1, 0, 0));
		double prevFrameTime = glfwGetTime();
		while (!glfwWindowShouldClose(window))
		{
			double currentFrameTime = glfwGetTime();
			ImGui_ImplGlfwGL3_NewFrame();
			glfwGetCursorPos(window, &mouseXnew, &mouseYnew);

#pragma region input
			i++;
			rot += 0.005f;

			float camSpeed = 0.05f;
			glm::vec3 camMovement = glm::vec3();
			const glm::vec3 forward = camera.GetForward();
			const glm::vec3 up = camera.GetUp();

			int mb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
			ImGui::Text("mouse click %s", mb ? "true" : "false");
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camMovement += camSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camMovement -= camSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				camMovement += camSpeed * up;
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				camMovement -= camSpeed * up;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camMovement -= glm::normalize(glm::cross(forward, up)) * camSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camMovement += glm::normalize(glm::cross(forward, up)) * camSpeed;
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				camera.RotateYlocal(.01f);
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				camera.RotateYlocal(-.01f);
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
				camera.RotateXlocal(.01f);
			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
				camera.RotateXlocal(-.01f);
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				lightpos += 2 * camSpeed * glm::vec3(-1, 0, 0);
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				lightpos += 2 * camSpeed * glm::vec3(1, 0, 0);
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				lightpos += 2 * camSpeed * glm::vec3(0, 0, -1);
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				lightpos += 2 * camSpeed * glm::vec3(0, 0, 1);
			if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
				lightpos += 2 * camSpeed * glm::vec3(0, 1, 0);
			if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
				lightpos += 2 * camSpeed * glm::vec3(0, -1, 0);


			*camera.Position() += camMovement;

			float signX = (float)glm::sign(mouseXnew - mouseXold);
			float signY = (float)glm::sign(mouseYold - mouseYnew);
			glm::vec2 mDiff = glm::vec2(signX, signY);
			glm::vec2 mouseVelocity(mouseXnew - mouseXold, mouseYnew - mouseYold);

			const glm::mat4 mvp = t.GetMVP(camera);

			ImGui::Text("mouse pos {x:%.2f, y:%.2f}", mDiff.x, mDiff.y);

			mouseXold = mouseXnew;
			mouseYold = mouseYnew;

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

			float deltaTime = currentFrameTime - prevFrameTime;
			output = true;

			//obj.meshes[0].m_animator.UpdateAnimation(deltaTime);
			obj.Draw(camera);
			cube.Draw(camera);
			plane.Draw(camera);

			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

			//lineshader.Bind();
			//GLCall(glBindVertexArray(lineVao));
			//GLCall(glDrawArrays(GL_LINES, 0, 4));

			///# 
			///# DRAW BONES
			///#


			//	boneshader.Bind();
			//	glEnable(GL_PROGRAM_POINT_SIZE);
			//	glPointSize(15);
			//
			//	boneshader.SetUniformMat4f("model", glm::mat4(1.0f));
			//	boneshader.SetUniformMat4f("view", camera.GetViewMatrix());
			//	boneshader.SetUniformMat4f("projection", camera.GetProjectionMatrix());
			//	GLCall(glBindVertexArray(boneVAO));
			//	GLCall(glDrawArrays(GL_POINTS, 0, boneverts.size()));
			//
			//	boneshader.Unbind();

			prevFrameTime = currentFrameTime;

			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			// clear all relevant buffers
			//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			//	glClear(GL_COLOR_BUFFER_BIT);
			ImGui::ColorEdit4("clear color", (float*)&override_color[0]);





			///	timer += 0.01f;
			postProcessShader.Bind();

			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	   //	back.Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//screenShader.Unbind();


			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			//glfwSwapBuffers(window);
			//glfwPollEvents();
		   ///




			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currentFrameTime);
			frametimes.push_back((float)diffms);
		}
	}

	ShaderManager::Destroy();
	//ImGui_ImplGlfwGL3_Shutdown();
	//ImGui::DestroyContext();

	//delete shaders while context is still current
	//auto& s = ShaderManager::getInstance() ;
	//s.reset();


	glfwTerminate();
	return 0;
}
unsigned int quadVAO = 0;
unsigned int quadVBO;


