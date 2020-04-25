#pragma once

class Camera;
class Model;

class InputManager
{
public:
	static void SetWindow(GLFWwindow* window);
	static void Update(Camera& camera);
	static Model* GetSelectedModel() { return GetInstance().m_selectedModel; }
	static void SetSelectedModel(Model* model) { GetInstance().m_selectedModel = model; }
	static bool m_isClicked;
private:
	InputManager();

	static InputManager& GetInstance();

	GLFWwindow* m_window;
	glm::mediump_f64vec2 m_mouseOld;
	glm::mediump_f64vec2 m_mouseNew;
	Model* m_selectedModel;

};

