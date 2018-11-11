#include "precomp.h"

Transform::Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)
{
	this->pos = pos;
	this->rot = rot;
	this->scale = scale;
}

//Transform::Rotate(float degrees, glm::vec3 vec)
//{
//	
//}
Transform::~Transform() = default;
