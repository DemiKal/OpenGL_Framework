#pragma once

class Cube;

class Camera
{
public:
	Camera() : fov(70), aspectRatio((float)SCREENWIDTH / (float)SCREENHEIGHT),
		pos(glm::vec3(0, 0, 0)), forward(glm::vec3(0, 0, -1)),
		up(glm::vec3(0, 1, 0)), projection(glm::perspective(70.f, aspectRatio, 0.1f, 200.0f)) {};

	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	void MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2& mvelo);
	glm::vec3 RayFromMouse(float mouseX, float mouseY) const;
	void SetOrthographic();


	static void SetMainCamera(Camera* cam)
	{
		//Camera* maincam = GetMain();
		m_mainCam = cam;
	}


	static Camera* GetMain()
	{
		return   m_mainCam;
	}

	inline glm::mat4 GetViewProjectionMatrix() const
	{
		return projection * glm::lookAt(pos, pos + forward, up);
	}

	void meme(Cube& cyb);

	void CheckMouseHover(float mX, float mY, const Cube& cube);


	inline glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(pos, pos + forward, up);
	}

	inline glm::mat4 GetProjectionMatrix() const
	{
		return projection;
	}

	//not working
	void SetPerspective(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);

	~Camera();

	glm::vec3* Position() { return  &pos; }
	glm::vec3 PositionRead() const { return pos; };
	float GetAspectRatio() const { return  aspectRatio; }
	void SetAspectRatio(const float asp) { aspectRatio = asp; } //possible extra stuff to do when setting
	glm::vec3 GetForward() const;

	//yaw
	void RotateXlocal(float angle)
	{
		const glm::vec3 right = glm::normalize(glm::cross(up, forward));
		const glm::mat4 rot = glm::rotate(glm::mat4(1), angle, right);

		forward = glm::vec3(glm::normalize(rot * glm::vec4(forward, 0.0)));
		up = glm::normalize(glm::cross(forward, right));
	}

	//rotate around the y axis, its own up vector
	void RotateYlocal(float angle)
	{
		static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(glm::mat4(1), angle, up);
		forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));

		up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
	}

	glm::vec3 GetUp() const;

private:
	float fov;
	float aspectRatio;
	glm::vec3 pos{};
	glm::vec3 forward{};
	glm::vec3 up{};
	glm::mat4 projection{};
	static Camera* m_mainCam;// = new Camera();
};

