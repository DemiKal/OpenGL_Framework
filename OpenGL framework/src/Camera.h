#pragma once
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
//
struct Camera
{
public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar);
	void SetOrthographic();

	inline glm::mat4 GetViewProjection() const
	{
		return projection * glm::lookAt(pos, pos + forward, up);
	}

	void SetPerspective(const glm::vec3 & pos, float fov, float aspect, float zNear, float zFar);

	~Camera();

	glm::vec3* Position() { return  &pos; }
	float GetAspectRatio() const { return  aspectRatio; }
	float SetAspectRatio(float asp) { aspectRatio = asp; } //possible extra stuff to do when setting
	glm::vec3 GetForward() const;

	//void MoveForward(float amt)
	//{
	//	pos += forward * amt;
	//}

	//void MoveRight(float amt)
	//{
	//	pos += glm::cross(up, forward) * amt;
	//}

	void Pitch(float angle)
	{
		glm::vec3 right = glm::normalize(glm::cross(up, forward));

		forward = glm::vec3(glm::normalize(
			glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 0.0)));
		up = glm::normalize(glm::cross(forward, right));
	}

	void RotateY(float angle)
	{
		static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(glm::mat4(1), angle, UP);

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

