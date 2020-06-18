#pragma once
#include "Geometry/Ray.h"

class Model;

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	~Camera();

	[[nodiscard]] Ray RayFromMouse(double mouseX, double mouseY) const;

	static void SetCamera2(Camera* camera) { m_cam2 =  camera; }
	static void SetMainCamera(Camera* cam) { m_mainCam = cam; }

	static Camera* GetMain() { return   m_mainCam; }
	static Camera* GetCam2() { return   m_cam2; }

	//not working
	void SetPerspective(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	void SetOrthographic();
	void SetAspectRatio(const float asp) { m_aspectRatio = asp; } //possible extra stuff to do when setting
	void MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2& mouseVelocity);
	void RotateLocalX(float angle); 	//yaw 
	void RotateLocalY(float angle);	//rotate around the y axis, its own up vector
	void SetViewVector(const glm::vec3& view);
	[[nodiscard]] glm::vec3& GetPosition() { return m_pos; }
	[[nodiscard]] glm::vec3 PositionRead() const { return m_pos; };
	[[nodiscard]] float GetAspectRatio() const { return  m_aspectRatio; }
	[[nodiscard]] glm::vec3 GetForwardVector() const { 	return m_forward; }
	[[nodiscard]] glm::vec3 GetUpVector() const;
	[[nodiscard]] float GetNearPlaneDist() const;
	[[nodiscard]] float GetFarPlaneDist() const;
	[[nodiscard]] inline glm::mat4 GetViewProjectionMatrix() const;
	[[nodiscard]] inline glm::mat4 GetViewMatrix() const;
	[[nodiscard]] inline glm::mat4 GetProjectionMatrix() const;
	[[nodiscard]] std::pair<bool, Model*> MousePick(double MouseX, double MouseY) const; //TODO fix without pointer and use optional
	 
private:
	float m_fov;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;
	glm::vec3 m_pos{};
	glm::vec3 m_forward{};
	glm::vec3 m_up{};
	glm::mat4 m_projection{};
	static Camera* m_mainCam;
	static Camera* m_cam2;
	
};

