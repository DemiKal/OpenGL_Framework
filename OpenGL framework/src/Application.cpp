#include "precomp.h"

int main(void)
{
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow * window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", NULL, NULL);
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

		//Cube cube("myCube");

		//const std::string path = "res/meshes/cyborg/cyborg.obj";
		// const std::string path = ;

		//GameObject bunny("Spyro");
		//bunny.renderer = renderer;
		//bunny.LoadMesh(path);

		//Texture tex("res\textures\uvtest.png", Texture::DIFFUSE);

		///test
		 //Model obj = Model("res/meshes/nanosuit/nanosuit.obj");
		 //obj.SetShader("normalmapshader");

		//Model obj2 = Model("res/meshes/cyborg/cyborg.obj");
		//	obj2.SetShader("normalmapshader");


		float cubeVertices[] = {
			// positions          // texture Coords
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		float planeVertices[] = {
			// positions          // texture Coords 
			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

			 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
			 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
		};
		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		// cube VAO
		unsigned int cubeVAO, cubeVBO;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		// plane VAO
		unsigned int planeVAO, planeVBO;
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		// screen quad VAO
		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// load textures
		// -------------
		Texture t1("res/textures/marble.jpg");
		Texture t2("res/textures/metal.png");



		unsigned int cubeTexture = t1.GetID();
		unsigned int floorTexture = t2.GetID();

		int shader_idx = ShaderManager::getShaderIdx("framebuffers");
		int screenShader_idx = ShaderManager::getShaderIdx("framebuffers_screen");

		auto& shader = ShaderManager::getShaderIdx(shader_idx);
		auto& postProcessShader = ShaderManager::getShaderIdx(screenShader_idx);

		shader.Bind();
		shader.SetInt("texture1", 0);

		postProcessShader.Bind();
		postProcessShader.SetInt("screenTexture", 0);


		// framebuffer configuration
		// -------------------------
		////////////////////////////

		 FrameBuffer framebuffer;
		//unsigned int fb;
		//glGenFramebuffers(1, &fb);
		//glBindFramebuffer(GL_FRAMEBUFFER, fb);


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








		const float aspect = (float)SCREENWIDTH / (float)SCREENHEIGHT;

		Camera camera(glm::vec3(0, 0, 3), 70, aspect, 0.1f, 200.0f);

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

		Transform t;
		int i = 0;
		std::vector<float>  frametimes;

		glm::vec3 lightpos(1.0f, 2.0f, 3.0f);
		glm::mat4 pp = glm::translate(glm::mat4(1.0f), lightpos);
		glm::vec4 s = pp[3];
		glm::vec4  override_color (0.3f, 0.5, 0.1, 0.5f);

		//
		// Render loop
		//
		float tick = 0;
		float mult = 1;
		while (!glfwWindowShouldClose(window))
		{
			tick += mult * 0.3f;
			if (tick >= 45.0f)
			{
				tick = 45.0f;
				mult *= -1;
			}
			else if (mult <= 0.f)
			{
				tick = 0.0;
				mult *= -1;
			}

			tick = 45;
			ImGui_ImplGlfwGL3_NewFrame();

			i++;
			double currenttime = glfwGetTime();

			rot += 0.005f;
			glfwGetCursorPos(window, &mouseXnew, &mouseYnew);

			float camSpeed = 0.05f;
			glm::vec3 camMovement = glm::vec3();
			const glm::vec3 forward = camera.GetForward();
			const glm::vec3 up = camera.GetUp();

			int mb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
			ImGui::Text("mouse click %s", mb ? "true" : "false");

#pragma region input
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
				lightpos += 2 * camSpeed* glm::vec3(-1, 0, 0);
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				lightpos += 2 * camSpeed*glm::vec3(1, 0, 0);
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				lightpos += 2 * camSpeed*glm::vec3(0, 0, -1);
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				lightpos += 2 * camSpeed*glm::vec3(0, 0, 1);
			if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
				lightpos += 2 * camSpeed*glm::vec3(0, 1, 0);
			if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
				lightpos += 2 * camSpeed*glm::vec3(0, -1, 0);


			*camera.Position() += camMovement;

			float signX = (float)glm::sign(mouseXnew - mouseXold);
			float signY = (float)glm::sign(mouseYold - mouseYnew);
			glm::vec2 mDiff = glm::vec2(signX, signY);
			glm::vec2 mouseVelocity(mouseXnew - mouseXold, mouseYnew - mouseYold);
#pragma endregion input

			int a = 1;
			int b = a << 3;


			//	renderer.Clear();
				///test / (float)

			const glm::mat4 mvp = t.GetMVP(camera);
			//auto model = glm::mat4(1.0f);
			//model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions

			//const glm::mat4 view = camera.GetViewMatrix();
			//const glm::mat4 proj = camera.GetProjectionMatrix();
			//const glm::vec3 viewpos = *camera.Position();

			//LightObj.Draw(camera);
			//obj2.Draw(camera);


			ImGui::Text("mouse pos {x:%.2f, y:%.2f}", mDiff.x, mDiff.y);


			mouseXold = mouseXnew;
			mouseYold = mouseYnew;

			const float average = (float)std::accumulate(frametimes.begin(), frametimes.end(), 0.0) / frametimes.size();
			const float avgfps = 1000.0f * float(1.0f / average);

			ImGui::Text("ms %f", avgfps);

			if (frametimes.size() >= 30)
			{
				frametimes.clear();
			}

			//glBindFramebuffer(GL_FRAMEBUFFER, fb);

			framebuffer.Bind();
			//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			// make sure we clear the framebuffer's content
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			//obj.Draw(camera);

			shader.Bind();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(tick),
				(float)SCREENWIDTH / (float)SCREENHEIGHT, 0.1f, 100.0f);

			shader.SetUniformMat4f("view", view);
			shader.SetUniformMat4f("projection", projection);
			// cubes
			glBindVertexArray(cubeVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cubeTexture);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
			shader.SetUniformMat4f("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
			shader.SetUniformMat4f("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// floor
			glBindVertexArray(planeVAO);
			glBindTexture(GL_TEXTURE_2D, floorTexture);
			shader.SetUniformMat4f("model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			//shader.Unbind();

			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			// clear all relevant buffers
			//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			//	glClear(GL_COLOR_BUFFER_BIT);
			ImGui::ColorEdit4("clear color", (float*)&override_color[0]);

			postProcessShader.Bind();
			postProcessShader.SetVec4f("override_color", override_color);

			//		screenShader.setVec3("override_color", glm::vec3(1.0f, 0.0f, 0.0f));
			glBindVertexArray(quadVAO);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
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
			const double diffms = 1000 * (endtime - currenttime);
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


