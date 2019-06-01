#pragma once

class Cube;

struct Camera
{
public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	void MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2 & mvelo);
	glm::vec3 RayFromMouse(float mouseX, float mouseY) const;
	void SetOrthographic();

	inline glm::mat4 GetViewProjectionMatrix() const
	{
		return projection * glm::lookAt(pos, pos + forward, up);
	}

	void meme(Cube& cyb);

	void CheckMouseHover(double mX, double mY, Cube & cube);
	

	inline glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(pos, pos + forward, up);
	}

	inline glm::mat4 GetProjectionMatrix() const
	{
		return projection;
	}

	//not working
	void SetPerspective(const glm::vec3 & pos, float fov, float aspect, float zNear, float zFar);

	~Camera();

	glm::vec3* Position() { return  &pos; }
	glm::vec3 PositionRead() const { return pos; };
	float GetAspectRatio() const { return  aspectRatio; }
	void SetAspectRatio(const float asp) { aspectRatio = asp; } //possible extra stuff to do when setting
	glm::vec3 GetForward() const;

	//void MoveForward(float amt)
	//{
	//	pos += forward * amt;
	//}

	//void MoveRight(float amt)
	//{
	//	pos += glm::cross(up, forward) * amt;
	//}

	//yaw
	void RotateXlocal (float angle)
	{
		const glm::vec3 right = glm::normalize(glm::cross(up, forward));
		const glm::mat4 rot = glm::rotate(glm::mat4(1), angle, right);

		forward = glm::vec3(glm::normalize(rot * glm::vec4(forward, 0.0)));
		up = glm::normalize(glm::cross(forward, right));
	}

	//rotate around the y axis, its own up vector
	void RotateYlocal (float angle)
	{
		static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(glm::mat4(1), angle, up);
		forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
		
		up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
	}

	glm::vec3 GetUp() const;

protected:
private:
	glm::mat4 projection{};
	glm::vec3 pos{};
	glm::vec3 forward{};
	glm::vec3 up{};
	float fov;
	float aspectRatio;
};

