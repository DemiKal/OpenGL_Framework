#pragma once
#include "Geometry/Ray.h"

class Model;

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	void MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2& mvelo);
	[[nodiscard]] Ray RayFromMouse(double mouseX, double mouseY) const;
	void SetOrthographic();

	static void SetCamera2(Camera* camera) { m_cam2 =  camera; }
	static void SetMainCamera(Camera* cam) { m_mainCam = cam; }

	static Camera* GetMain() { return   m_mainCam; }
	static Camera* GetCam2() { return   m_cam2; }

	[[nodiscard]] inline glm::mat4 GetViewProjectionMatrix() const;

	std::pair<bool, Model*> MousePick(double MouseX, double MouseY) const;

	[[nodiscard]] inline glm::mat4 GetViewMatrix() const;

	[[nodiscard]] inline glm::mat4 GetProjectionMatrix() const;

	//not working
	void SetPerspective(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);

	~Camera();

	[[nodiscard]] glm::vec3& GetPosition() { return pos; }
	[[nodiscard]] glm::vec3 PositionRead() const { return pos; };
	[[nodiscard]] float GetAspectRatio() const { return  aspectRatio; }
	void SetAspectRatio(const float asp) { aspectRatio = asp; } //possible extra stuff to do when setting
	[[nodiscard]] glm::vec3 GetForwardVector() const
	{
		return forward;
	}
	[[nodiscard]] glm::vec3 GetUpVector() const;

	//yaw 
	void RotateXlocal(const float angle);

	//rotate around the y axis, its own up vector
	void RotateYlocal(const float angle);


private:
	float fov;
	float aspectRatio;
	glm::vec3 pos{};
	glm::vec3 forward{};
	glm::vec3 up{};
	glm::mat4 projection{};
	static Camera* m_mainCam;
	static Camera* m_cam2;
};

