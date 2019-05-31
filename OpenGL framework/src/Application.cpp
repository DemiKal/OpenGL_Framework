#include "precomp.h"
void renderQuad();


//struct Cuban;
int main(void)
{
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

		//Cube cube("myCube");

		const std::string path = "res/meshes/cyborg/cyborg.obj";
		//const std::string path = "res/meshes/nanosuit/nanosuit.obj";

		//GameObject bunny("Spyro");
		//bunny.renderer = renderer;
		//bunny.LoadMesh(path);

		///test
		Model mdl(path);
		Shader shader("res/shaders/normalmapshader.glsl");
		shader.Bind();
		// shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.0f);
		//shader.SetUniform1i("diffuseMap", 0);
		//shader.SetUniform1i("normalMap", 1);

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
		std::clock_t start;
		double duration;

		Transform t;

		int i = 0;
		std::vector<float>  frametimes;


		//Texture2D diffuseMap = Texture2D("res/textures/brickwall.jpg", "texture_diffuse");
		//Texture2D normalMap = Texture2D("res/textures/brickwall_normal.jpg", "texture_normal");

		  glm::vec3 lightpos( 0.5f, 1.0f, 0.3f)  ;


		while (!glfwWindowShouldClose(window))
		{
			ImGui_ImplGlfwGL3_NewFrame();

			i++;
			double currenttime = glfwGetTime();

			rot += 0.005;
			glfwGetCursorPos(window, &mouseXnew, &mouseYnew);

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
				lightpos += 2*camSpeed* glm::vec3(-1, 0, 0);
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

			float signX = glm::sign(mouseXnew - mouseXold);
			float signY = glm::sign(mouseYold - mouseYnew);
			glm::vec2 mDiff = glm::vec2(signX, signY);
			glm::vec2 mouseVelocity(mouseXnew - mouseXold, mouseYnew - mouseYold);


			renderer.Clear();
			///test / (float)
			//glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			//	(float)SCREENWIDTH / (float)SCREENHEIGHT, 0.1f, 100.0f);
			//
			//const glm::mat4 view = camera.GetViewMatrix();
			//
			//shader.SetUniformMat4f("projection", projection);
			//shader.SetUniformMat4f("view", view);
			//
			//// render normal-mapped quad
			//glm::mat4 model = glm::mat4(1.0f);
			// model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
			//shader.SetUniformMat4f("model", model);
			//glm::vec3 viewpos = *camera.Position();
			//shader.setVec3("viewPos", viewpos);
			//shader.setVec3("lightPos", { 0.5f, 1.0f, 0.3f });
			//GLCall(glActiveTexture(GL_TEXTURE0);				  )
			//GLCall(glBindTexture(GL_TEXTURE_2D, diffuseMap.GetID()));
			//GLCall(glActiveTexture(GL_TEXTURE1);				  )
			//GLCall(glBindTexture(GL_TEXTURE_2D, normalMap.GetID()));
			//renderQuad();

			const glm::mat4 mvp = t.GetMVP(camera);
			auto model = glm::mat4(1.0f);
			//model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions

			const glm::mat4 view = camera.GetViewMatrix();
			const glm::mat4 proj = camera.GetProjectionMatrix();
			const glm::vec3 viewpos = *camera.Position();

			shader.SetUniformMat4f("model", model);
			shader.SetUniformMat4f("projection", proj);
			shader.SetUniformMat4f("view", view);
			
			shader.setVec3("viewPos", viewpos);
			shader.setVec3("lightPos", lightpos);


			mdl.Draw(shader);
			//draw old style
			//const glm::mat4 mvp = bunny.m_transform->GetMVP(camera);
			//bunny.m_shader->Bind();
			//bunny.m_shader->SetUniformMat4f("u_MVP", mvp);
			//bunny.Draw();
			  //cube.Draw ();
			//mesh.Draw();
					   //camera.CheckMouseHover(mouseXnew, mouseYnew, cube);
					   // camera.meme(cube);


	   //{
	   //	static float f = 0.0f;
	   //	static int counter = 0;
	   //	ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
	   //	ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
	   //	ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
	   //	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
	   //	//ImGui::Checkbox("Another Window", &show_another_window);
	   //	if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
	   //		counter++;
	   //	ImGui::SameLine();
	   //	ImGui::Text("counter = %d", counter);
		   //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("mouse pos {x:%.2f, y:%.2f}", mDiff.x, mDiff.y);
			//}

			mouseXold = mouseXnew;
			mouseYold = mouseYnew;

			const float average = std::accumulate(frametimes.begin(), frametimes.end(), 0.0) / frametimes.size();
			const float avgfps = 1000 * float(1.0 / average);

			ImGui::Text("ms %f", avgfps);

			if (frametimes.size() >= 30)
			{
				frametimes.clear();
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currenttime);
			frametimes.push_back(diffms);
		}
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1(0, 0, 0), bitangent1(0, 0, 0);
		glm::vec3 tangent2(0, 0, 0), bitangent2(0, 0, 0);
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
