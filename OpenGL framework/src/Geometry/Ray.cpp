#include "precomp.h"
#include "Ray.h"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction):
	m_origin(origin),
	m_direction(direction),
	t(0)
{
}

glm::vec3 Ray::Direction() const
{
	return m_direction;
}

glm::vec3 Ray::Origin() const
{
	return m_origin;
}
