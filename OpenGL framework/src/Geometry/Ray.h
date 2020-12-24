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
	Ray operator*(const glm::mat4& lhs) const
	{
		const glm::vec3 newOrigin = lhs * glm::vec4(m_origin, 1);
		const glm::vec3 newDir = lhs * glm::vec4(m_origin, 0);
		return{ newOrigin, newDir };
	}
};


