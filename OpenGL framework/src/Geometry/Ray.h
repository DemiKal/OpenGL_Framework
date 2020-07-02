#pragma once
class Ray
{
public:
	glm::vec3 m_origin;
	glm::vec3 m_direction;
	float t;

	Ray(const glm::vec3& origin, const glm::vec3& direction);

	glm::vec3 Direction() const;
	glm::vec3 Origin() const;
};


