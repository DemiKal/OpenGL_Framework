#include "precomp.h"
#include "Camera.h"
#include "Components/AABB.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/Model.h"

Camera* Camera::m_mainCam;
Camera* Camera::m_cam2;

Camera::Camera() :
	fov(70), 
	aspectRatio((float)SCREENWIDTH / (float)SCREENHEIGHT), //TODO: fix scwidht!
	pos(glm::vec3(0, 0, 0)), 
	forward(glm::vec3(0, 0, -1)),
	up(glm::vec3(0, 1, 0)), 
	m_nearPlane(0.1f),
	m_farPlane(200.0f),
	projection(glm::perspective(fov, aspectRatio, m_nearPlane, m_farPlane)) {}

Camera::Camera(const glm::vec3& p_pos,
	const float p_fov,
	const float p_aspect,
	const float p_zNear,
	const float p_zFar)
	:
	fov(p_fov),
	aspectRatio(p_aspect),
	pos(p_pos),
	forward(glm::vec3(0, 0, -1)),
	up(glm::vec3(0, 1, 0)),
	m_nearPlane(p_zNear),
	m_farPlane(p_zFar),
	projection(glm::perspective(p_fov, p_aspect, p_zNear, p_zFar))
{

}

Camera::~Camera() = default;

void Camera::MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2 & mvelo)
{
	mDiff = glm::normalize(mDiff);
	if (glm::abs(mDiff.x) < 10 || glm::abs(mDiff.y) < 10)
	{
		RotateYlocal(-mDiff.x * camSpeed / 2 * glm::length(mvelo));
		RotateXlocal(-mDiff.y * camSpeed / 2 * glm::length(mvelo));
	}
}

//TODO: use a screenmanager to get SCREENWIDTH dynamically!
Ray Camera::RayFromMouse(const double mouseX, const double mouseY) const
{
	const float x = (2.0f * mouseX) / SCREENWIDTH - 1.0f;
	const float y = 1.0f - (2.0f * mouseY) / SCREENHEIGHT;
	const float z = 1.0f;
	const glm::mat4 proj_mat = GetProjectionMatrix();
	const glm::mat4 view_matrix = GetViewMatrix();

	//normalized device coordinates [-1:1, -1:1, -1:1]
	const glm::vec3 ray_nds = glm::vec3(x, y, z);

	// clip space (4d homogenized) [-1:1, -1:1, -1:1, -1:1]
	const glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	// eye space [-x:x, -y:y, -z:z, -w:w]
	glm::vec4 ray_eye = glm::inverse(proj_mat) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space [-x:x, -y:y, -z:z, -w:w]
	glm::vec3 ray_world = glm::vec3(glm::inverse(view_matrix) * ray_eye);
	ray_world = glm::normalize(ray_world);

	Ray ray(this->PositionRead(), ray_world);

	return ray;
}

//TODO: set screenwidht and other vars dynamically!
void Camera::SetOrthographic()
{
	projection = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -10.0f, 100.0f);
}

inline glm::mat4 Camera::GetViewProjectionMatrix() const
{
	return projection * glm::lookAt(pos, pos + forward, up);
}




std::pair<bool, Model*> Camera::MousePick(double MouseX, double MouseY) const
{
	const Ray ray = RayFromMouse(MouseX, MouseY);
	//glm::vec3 origin = GetPosition();
	std::vector < std::pair<Model*, float>> hits;
	float t_min = 99999999;
	Model* m_current = nullptr;
	bool totalHit = false;

	for (Model* mdl : EntityManager::GetEntities())
	{
		float tCurrent = -1111;
		bool hit = mdl->meshes[0].m_aabb.IntersectAABB(ray, tCurrent);
		///bool hit = intersectAABB(origin, dir, tCurrent, mdl->meshes[0].m_aabb);
		totalHit |= hit;
		if (hit && tCurrent < t_min)
		{
			m_current = mdl;
			t_min = tCurrent;
		}
	}

	return { totalHit , m_current };
	//model = m_current;
	//
	//return totalHit;
	//std::min_element(hits.begin(), hits.end())
}

inline glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(pos, pos + forward, up);
}

inline glm::mat4 Camera::GetProjectionMatrix() const
{
	return projection;
}

void Camera::SetPerspective(const glm::vec3 & pos, float fov, float aspect, float zNear, float zFar)
{
	projection = glm::perspective(fov, aspect, zNear, zFar);
}


//yaw 
inline void Camera::RotateXlocal(const float angle)
{
	const glm::vec3 right = glm::normalize(glm::cross(up, forward));
	const glm::mat4 rot = glm::rotate(glm::mat4(1), angle, right);

	forward = glm::vec3(glm::normalize(rot * glm::vec4(forward, 0.0)));
	up = glm::normalize(glm::cross(forward, right));
}


//rotate around the y axis, its own up vector

inline void Camera::RotateYlocal(const float angle)
{
	static const glm::vec3 UP(0.0f, 1.0f, 0.0f);
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, up);
	forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));

	up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
}

float Camera::GetNearPlaneDist()
{
	return m_nearPlane;
}

float Camera::GetFarPlaneDist()
{
	return m_farPlane;
}

glm::vec3 Camera::GetUpVector() const
{
	return up;
}


