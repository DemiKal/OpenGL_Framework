#include "Camera.h"


Camera::Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar)
{
	this->pos = pos;
	this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->projection = glm::perspective(fov, aspect, zNear, zFar);

	this->aspectRatio = aspect;
	this->fov = fov;
}

void Camera::SetOrthographic()
{
	this->projection = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -10.0f, 100.0f);
}

void Camera::SetPerspective(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar)
{
	this->projection = glm::perspective(fov, aspect, zNear, zFar);
}
Camera::~Camera()
{
}

glm::vec3 Camera::GetForward() const
{
	return this->forward;
}

glm::vec3 Camera::GetUp() const
{
	return this->up;
}


