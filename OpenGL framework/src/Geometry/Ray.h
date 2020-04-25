#pragma once
class Ray
{
public:
	glm::vec3 m_origin;
	glm::vec3 m_direction;
	float t;

	Ray(const glm::vec3& origin, const glm::vec3& direction) :
		m_origin(origin),
		m_direction(direction),
		t(0) {}

	glm::vec3 Direction() const { return m_direction; }
	glm::vec3 Origin() const { return m_origin; }
};


