#pragma once

class Cube;

class Camera
{
public:
	Camera();;
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	void MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2& mvelo);
	glm::vec3 RayFromMouse(float mouseX, float mouseY) const;
	void SetOrthographic();

	static void SetMainCamera(Camera* cam) { m_mainCam = cam; }

	static Camera* GetMain() { return   m_mainCam; }

	inline glm::mat4 GetViewProjectionMatrix() const;

	void meme(Cube& cyb);

	void CheckMouseHover(float mX, float mY, const Cube& cube);

	inline glm::mat4 GetViewMatrix() const;

	inline glm::mat4 GetProjectionMatrix() const;

	//not working
	void SetPerspective(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);

	~Camera();

	glm::vec3& GetPosition() { return pos; }
	glm::vec3 PositionRead() const { return pos; };
	float GetAspectRatio() const { return  aspectRatio; }
	void SetAspectRatio(const float asp) { aspectRatio = asp; } //possible extra stuff to do when setting
	glm::vec3 GetForwardVector() const
	{
		return forward;
	}
	glm::vec3 GetUpVector() const;

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
};

