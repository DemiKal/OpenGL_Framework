#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "tests/TestClearColor.h"
#include "../GameObject.h"
#include "../Cube.h"

static const int SCREENWIDTH = 1280;
static const int SCREENHEIGHT = 720;

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) std::cout << "ERROR!" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	//glFrontFace(GL_CW);
	{ 
		//GLCall(glEnable(GL_BLEND));
		//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
 
		Renderer renderer; 
		Cube cube("myCube");
		cube.renderer = &renderer;
		 
		//const glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -10.0f, 100.0f);
		// Camera camera(glm::vec3(0.0f, 0.0f, -5.0f), 70.0f, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.1f, 100.0f);
		float aspect = (float)SCREENWIDTH / (float)(SCREENHEIGHT);

		const glm::mat4 proj = glm::perspective(70.0f, aspect, 0.1f, 200.0f);

		//glm::mat4 view = translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		glm::vec3 camPos(0, 0, 5);
		glm::vec3 forward(0, 0, -1);
		glm::vec3 up(0, 1, 0);

		glm::mat4 view = lookAt(
			camPos,
			camPos + forward,
			up
		);
		 
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		glm::vec3 translationA(0, 0, 0);
		glm::vec3 translationB(-1, -1, 0);

		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear color");
		float rot = 0;
		//game loop
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);

		while (!glfwWindowShouldClose(window))
		{
			rot += 0.005;
			renderer.Clear();
			//ImGui_ImplGlfwGL3_NewFrame();
			ImGui_ImplGlfwGL3_NewFrame();

			float cameraSpeed = 0.05f;

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camPos += cameraSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camPos -= cameraSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camPos -= glm::normalize(glm::cross(forward, up)) * cameraSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camPos += glm::normalize(glm::cross(forward, up)) * cameraSpeed;


			view = lookAt(
				camPos,
				camPos + forward,
				up
			);

			if (currentTest)
			{
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				ImGui::Begin("Test");
				if (currentTest != testMenu && ImGui::Button("<-"))
				{
					delete currentTest;
					currentTest = testMenu;
				}

				currentTest->OnImGuiRender();
				ImGui::End();
			}



			{
				auto r = glm::rotate(glm::mat4(1), rot, glm::vec3(0, 1, 0));

				glm::mat4 model = r;//translate(glm::mat4(1.0f), translationA); //* glm::rotate(glm::mat4(1.0f), 0,glm::vec3(0, 1, 0));
 

				//glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				//cube.m_shader.Bind();
				cube.m_shader->SetUniformMat4f("u_MVP", mvp);
				cube.Draw();
			}
 
		 

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
			//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			//}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());
		}

		delete currentTest;
		if (currentTest != testMenu)
			delete testMenu;
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
