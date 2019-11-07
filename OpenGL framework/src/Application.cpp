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

		//const std::string path = "res/meshes/cyborg/cyborg.obj";
		// const std::string path = ;

		//GameObject bunny("Spyro");
		//bunny.renderer = renderer;
		//bunny.LoadMesh(path);

		ShaderManager::Init("res/shaders"); //init before any model

		///test
		// Model obj = Model("res/meshes/nanosuit/nanosuit.obj");
		// obj.SetShader("normalmapshader");
		 
		Model obj2 = Model("res/meshes/cyborg/cyborg.obj");
		obj2.SetShader("normalmapshader"); 
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

		glm::vec3 lightpos(0.5f, 1.0f, 0.3f);


		while (!glfwWindowShouldClose(window))
		{
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


			renderer.Clear();
			///test / (float)

			const glm::mat4 mvp = t.GetMVP(camera);
			auto model = glm::mat4(1.0f);
			//model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions

			const glm::mat4 view = camera.GetViewMatrix();
			const glm::mat4 proj = camera.GetProjectionMatrix();
			const glm::vec3 viewpos = *camera.Position();

			//LightObj.Draw(camera);
			//obj.Draw(camera);
			obj2.Draw(camera);


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

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			const double endtime = glfwGetTime();
			const double diffms = 1000 * (endtime - currenttime);
			frametimes.push_back((float)diffms);
		}
	}

	ShaderManager::Destroy();
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	//delete shaders while context is still current
	//auto& s = ShaderManager::getInstance() ;
	//s.reset();


	glfwTerminate();
	return 0;
}
unsigned int quadVAO = 0;
unsigned int quadVBO;

 
