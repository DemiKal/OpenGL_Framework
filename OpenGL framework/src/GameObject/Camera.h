#pragma once

class Model;
class Ray;

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	~Camera() = default;


	static void SetCamera2(Camera* camera);
	static void SetMainCamera(Camera* cam);

	[[nodiscard]] static Camera* GetMain();
	[[nodiscard]] static Camera* GetCam2();

	//not working [???]
	void LookAt(const glm::vec3& target);
	void MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2& mouseVelocity);
	void RecalcProjection();
	void Roll(float angle);
	void RotateLocalX(float angle); 	//yaw 
	void RotateLocalY(float angle);	//rotate around the y axis, its own up vector
	void SetAspectRatio(const float asp) { m_aspectRatio = asp; } //possible extra stuff to do when setting
	void SetOrthographic(float halfWidth, float halfHeight, float near, float far);
	void SetPerspective(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	void SetUpVector(const glm::vec3& up);
	void SetViewVector(const glm::vec3& view);

	[[nodiscard]] float GetAspectRatio() const;
	[[nodiscard]] float& GetFieldOfView();
	[[nodiscard]] float& GetNearPlaneDist();
	[[nodiscard]] float& GetFarPlaneDist();
	[[nodiscard]] glm::vec3& GetPosition();
	[[nodiscard]] glm::vec3 PositionRead() const;
	[[nodiscard]] glm::vec3 GetForwardVector() const;
	[[nodiscard]] glm::vec3 GetUpVector() const;
	[[nodiscard]] glm::mat4 GetViewProjectionMatrix() const;
	[[nodiscard]] inline glm::mat4 GetViewMatrix() const;
	[[nodiscard]] inline glm::mat4 GetProjectionMatrix() const;
	[[nodiscard]] std::pair<bool, Model*> MousePick(double MouseX, double MouseY) const; //TODO fix without pointer and use optional
	[[nodiscard]] Ray RayFromMouse(double mouseX, double mouseY) const;

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
	bool m_isOrthographic{};
};

