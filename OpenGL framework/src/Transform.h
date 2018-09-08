#pragma once
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"

struct Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(),
			  const glm::vec3& rot = glm::vec3(),
			  const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));
	
	~Transform();

	inline glm::mat4 GetModelMatrix() const
	{
		const glm::mat4 posMat = glm::translate(glm::mat4(1), pos);
		const glm::mat4 scaleMat = glm::scale(glm::mat4(1), scale);
		const glm::mat4 rotX = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1.0, 0.0, 0.0));
		const glm::mat4 rotY = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0.0, 1.0, 0.0));
		const glm::mat4 rotZ = glm::rotate(glm::mat4(1), rot.z, glm::vec3(0.0, 0.0, 1.0));
		const glm::mat4 rotMat = rotX * rotY * rotZ;

		return posMat * rotMat * scaleMat;
	}

	inline glm::mat4 GetMVP(const Camera& camera) const
	{
		glm::mat4 VP = camera.GetViewProjectionMatrix();
		glm::mat4 M = GetModelMatrix();

		return VP * M;
	}

	inline glm::vec3* GetPos() { return &pos; }
	inline glm::vec3* GetRot() { return &rot; }
	inline glm::vec3* GetScale() { return &scale; }

	inline void SetPos(glm::vec3& pos) { this->pos = pos; }
	inline void SetRot(glm::vec3& rot) { this->rot = rot; }
	inline void SetScale(glm::vec3& scale) { this->scale = scale; }
protected:
private:
	glm::vec3 pos{};
	glm::vec3 rot{};
	glm::vec3 scale{};
};