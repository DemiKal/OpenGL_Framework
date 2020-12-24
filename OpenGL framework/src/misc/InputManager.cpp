#include "precomp.h"
#include "InputManager.h"
#include "Geometry/Ray.h"
#include "GameObject/Camera.h"
#include "GameObject/Components/Model.h"
#include "Light/Light.h"
#include "Light/LightManager.h"
bool  InputManager::m_isClicked;

InputManager::InputManager() :
	m_window(nullptr),
	m_mouseOld(),
	m_mouseNew(),
	m_selectedModel(nullptr)
{
}

InputManager& InputManager::GetInstance()
{
	static InputManager instance;
	return instance;
}

void InputManager::SetWindow(GLFWwindow* window)
{
	GetInstance().m_window = window;
}


void InputManager::Update(Camera& camera, const float dt)
{
	//Camera& camera = Camera::GetMain();
	auto& instance = GetInstance();
	GLFWwindow* window = instance.m_window;
	instance.m_mouseOld = instance.m_mouseNew;

	glfwGetCursorPos(window, &instance.m_mouseNew.x, &instance.m_mouseNew.y);

	static float camSpeed = 30.0f;
	//ImGui::SliderFloat("cam speed", &camSpeed, 0.001f, 150.0f);

	static float turnSpeed = 2.0f;
	//ImGui::SliderFloat("Turn Speed", &turnSpeed, 0.001f, 15.0f);
	
	glm::vec3 camMovement = glm::vec3(0);
	const glm::vec3 forward = camera.GetForwardVector();
	const glm::vec3 up = camera.GetUpVector();

	const glm::vec3 upWorld = glm::vec3(0, 1, 0);
	const glm::vec3 rightWorld = glm::vec3(1, 0, 0);
	const glm::vec3 forwardWorld = glm::vec3(0, 0, -1);


	const float signX = (float)glm::sign(instance.m_mouseNew.x - instance.m_mouseOld.x);
	const float signY = (float)glm::sign(instance.m_mouseOld.y - instance.m_mouseNew.y);
	glm::vec2 mDiff = glm::vec2(signX, signY);
	//glm::vec2 mouseVelocity(mouseXnew - mouseXold, mouseYnew - mouseYold);

	

	
	int mb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
	//ImGui::Text("mouse click %s", mb ? "true" : "false");

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)			camMovement += dt * camSpeed * forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)			camMovement -= dt * camSpeed * forward;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)		camMovement += dt * camSpeed * 4 * up;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)			camMovement -= dt * camSpeed * up;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)			camMovement -= dt * glm::normalize(glm::cross(forward, up)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)			camMovement += dt * glm::normalize(glm::cross(forward, up)) * camSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)			camera.RotateLocalY(turnSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)			camera.RotateLocalY(-turnSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)			camera.RotateLocalX(turnSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)			camera.RotateLocalX(-turnSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)			camera.Roll(-turnSpeed * dt);

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)			LightManager::GetLight(0).GetPosition() += (2 * camSpeed * upWorld);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)			LightManager::GetLight(0).GetPosition() += 2 * camSpeed * -upWorld;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)		LightManager::GetLight(0).GetPosition() += 2 * camSpeed * -rightWorld;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)		LightManager::GetLight(0).GetPosition() += 2 * camSpeed * rightWorld;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)			LightManager::GetLight(0).GetPosition() += 2 * camSpeed * forwardWorld;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)		LightManager::GetLight(0).GetPosition() += 2 * camSpeed * -forwardWorld;
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)		LightManager::GetLight(0).GetPosition() += 2 * camSpeed * upWorld;
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)	LightManager::GetLight(0).GetPosition() += 2 * camSpeed * -upWorld;
	 

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		m_isClicked = true;
	}
	else m_isClicked = false;

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	//auto [hit, selected] = camera.MousePick(mouseX, mouseY);

	std::string selectedStr = "None";
	//if (hit)
	//{
	//selectedStr = selected->name;
	//instance.SelectedModel = selected;
	//}
	double MouseX, MouseY;

	glfwGetCursorPos(window, &MouseX, &MouseY);

	const Ray ray = camera.RayFromMouse(MouseX, MouseY,1920,1080);
	//bvh.TraceRay(ray);

	//ImGui::LabelText("label", selectedStr.c_str());

	//}

	camera.GetPosition() += camMovement;
}

Model* InputManager::GetSelectedModel()
{
	return GetInstance().m_selectedModel;
}

void InputManager::SetSelectedModel(Model* model)
{
	GetInstance().m_selectedModel = model;
}
