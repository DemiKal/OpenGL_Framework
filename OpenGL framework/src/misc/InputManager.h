#pragma once
class InputManager
{
public:
	static void SetWindow(GLFWwindow* window);
	static void Update(Camera& camera);

private:
	InputManager() {}
	GLFWwindow* m_window;
	static InputManager& GetInstance();
	glm::mediump_f64vec2 m_mouseOld;
	glm::mediump_f64vec2 m_mouseNew;
};

