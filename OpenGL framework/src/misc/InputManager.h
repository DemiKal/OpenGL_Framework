#pragma once

class Camera;
class Model;

class InputManager
{
public:
	static void SetWindow(GLFWwindow* window);
	static void Update(Camera& camera);
	static Model* GetSelectedModel() { return GetInstance().SelectedModel; }
	static void SetSelectedModel(Model* model) { GetInstance().SelectedModel = model; }
	static bool m_isClicked;
private:
	InputManager() {}
	GLFWwindow* m_window;
	static InputManager& GetInstance();
	glm::mediump_f64vec2 m_mouseOld;
	glm::mediump_f64vec2 m_mouseNew;
	Model* SelectedModel;

};

