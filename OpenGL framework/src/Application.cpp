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
#include "Camera.h"

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

	{
		Renderer renderer;
		Cube cube("myCube");
		cube.renderer = &renderer;

		float aspect = (float)SCREENWIDTH / (float)(SCREENHEIGHT);

		Camera camera(glm::vec3(0, 0, 5), 70, aspect, 0.1f, 200.0f);
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear color");
		float rot = 0;

		//game loop
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);

		//auto forward = glm::vec3(0, 0, -1);
		//auto up = glm::vec3(0, 1, 0);

		//auto l = glm::cross(forward, glm::vec3(1, 0, 0));
		double mouseXold = 0, mouseYold = 0;
		double mouseXnew = 0, mouseYnew = 0;
		while (!glfwWindowShouldClose(window))
		{
			rot += 0.005;
			glfwGetCursorPos(window, &mouseXnew, &mouseYnew);
			float signX = glm::sign(mouseXnew - mouseXold);
			float signY = glm::sign(mouseYold - mouseYnew);
			glm::vec2 mDiff = glm::vec2(signX, signY);
			

			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();

			float camSpeed = 0.05f;
			glm::vec3 camMovement = glm::vec3();
			const glm::vec3 forward = camera.GetForward();
			const glm::vec3 up = camera.GetUp();

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camMovement += camSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camMovement -= camSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camMovement += camSpeed * up;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camMovement -= glm::normalize(glm::cross(forward, up)) * camSpeed;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camMovement += glm::normalize(glm::cross(forward, up)) * camSpeed;

			//	camMovement += mDiff * camSpeed;

			*camera.Position() += camMovement;
			double currentTime = glfwGetTime();

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
				cube.m_shader->Bind();

				if (glm::abs(mDiff.x) < 10 || glm::abs(mDiff.y) < 10)
				{
					camera.RotateY(-mDiff.x*camSpeed/2 );
					camera.Pitch(-mDiff.y*camSpeed / 2);
				}

				glm::mat4 mvp = cube.m_transform->GetMVP(camera);
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
				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("mouse pos {x:%.2f, y:%.2f}", mDiff.x, mDiff.y);
			//}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			mouseXold = mouseXnew;
			mouseYold = mouseYnew;
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
