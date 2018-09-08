#define GLM_ENABLE_EXPERIMENTAL 

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
#include "mesh.h"
#include "glm/gtx/intersect.hpp";
//#include "glm/gtx/string_cast.inl"


static const int SCREENWIDTH = 1280;
static const int SCREENHEIGHT = 720;

glm::vec3 RayFromMouse(Camera& cam, float mouseX, float mouseY)
{
	float x = (2.0f * mouseX) / SCREENWIDTH - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / SCREENHEIGHT;
	float z = 1.0f;
	glm::mat4 proj_mat = cam.GetProjectionMatrix();
	glm::mat4 view_matrix = cam.GetViewMatrix();

	//normalized device coordinates [-1:1, -1:1, -1:1]
	glm::vec3 ray_nds = glm::vec3(x, y, z);

	// clip space (4d homogenized) [-1:1, -1:1, -1:1, -1:1]
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	// eye space [-x:x, -y:y, -z:z, -w:w]
	glm::vec4 ray_eye = glm::inverse(proj_mat) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space [-x:x, -y:y, -z:z, -w:w]
	glm::vec3 ray_world = glm::vec3(glm::inverse(view_matrix) * ray_eye);
	ray_world = glm::normalize(ray_world);
	//auto campos = glm::vec3(view_matrix[3]);
	//ImGui::Text("Cursor ray: %.2f %.2f %.2f", ray_world.x, ray_world.y, ray_world.z);
	//ImGui::Text("viewMat: %.2d %.2d %.2d", campos.x, campos.y, campos.z);

	return ray_world;
}

void CheckMouseHover(double mX, double mY, Camera& cam, Cube& cube)
{
	auto tris = cube.GetMeshTriangles();
	glm::vec3 const campos = *cam.Position();
	auto  dir = RayFromMouse(cam, mX, mY);

	// screen space (viewport coordinates)




	bool cubeIntersect = false;
	for (const auto tri : tris)
	{
		glm::vec2 baryCoords{ 0,0 };
		float t;

		const bool intersect = glm::intersectRayTriangle(campos,
			dir, (&tri)->v1, (&tri)->v2, (&tri)->v3, baryCoords, t);

		if (intersect)
		{
			cubeIntersect = true;
			break;
		}
	}


	std::string name = "none";

	name = cubeIntersect ? cube.Name()->c_str() : "none";

	ImGui::Text("Hovered obj: %s", name.c_str());
}

void MoveCameraMouse(Camera& camera, glm::vec2 mDiff, float camSpeed, glm::vec2& mvelo)
{
	mDiff = glm::normalize(mDiff);
	if (glm::abs(mDiff.x) < 10 || glm::abs(mDiff.y) < 10)
	{
		camera.RotateYlocal(-mDiff.x * camSpeed/2 * mvelo.length());
		camera.RotateXlocal(-mDiff.y * camSpeed/2 * mvelo.length() );
	}
	
}

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
		renderer.SetAlphaBlending(false);
		Cube cube("myCube");
		//Mesh mesh("res/mesh objects/weird2.obj");

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

		double mouseXold = 0, mouseYold = 0;
		double mouseXnew = 0, mouseYnew = 0;
		while (!glfwWindowShouldClose(window))
		{
			rot += 0.005;
			glfwGetCursorPos(window, &mouseXnew, &mouseYnew);
			float signX = glm::sign(mouseXnew - mouseXold);
			float signY = glm::sign(mouseYold - mouseYnew);
			glm::vec2 mDiff = glm::vec2(signX, signY);
			glm::vec2 mouseVelocity(mouseXnew - mouseXold, mouseYnew - mouseYold);

			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();

			float camSpeed = 0.05f;
			glm::vec3 camMovement = glm::vec3();
			const glm::vec3 forward = camera.GetForward();
			const glm::vec3 up = camera.GetUp();

			int mb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
			ImGui::Text("mouse click %s", mb ? "true" : "false");

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camMovement += camSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camMovement -= camSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camMovement += camSpeed * up;
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

				MoveCameraMouse(camera, mDiff, camSpeed, mouseVelocity);

				//*cube.m_transform->GetPos() += glm::vec3(0.01f, 0, 0);
				*cube.m_transform->GetRot() += glm::vec3(0.01f);

				//cubepos += glm::vec3(0.001f, 0, 0);

				glm::mat4 mvp = cube.m_transform->GetMVP(camera);
				cube.m_shader->SetUniformMat4f("u_MVP", mvp);
				cube.Draw();
				//mesh.Draw();

				CheckMouseHover(mouseXnew, mouseYnew, camera, cube);



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
