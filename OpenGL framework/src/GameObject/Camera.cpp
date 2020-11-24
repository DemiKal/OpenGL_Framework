#include "precomp.h"
#include "Camera.h"
#include "Components/AABB.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/Model.h"
#include "Geometry/Ray.h"

Camera* Camera::m_mainCam;
Camera* Camera::m_cam2;

Camera::Camera() :
	m_FOV(70),
	m_AspectRatio(static_cast<float>(SCREENWIDTH) / static_cast<float>(SCREENHEIGHT)), //TODO: fix scwidht!
	m_NearPlane(0.1f),
	m_FarPlane(200.0f),
	m_Pos(glm::vec3(0, 0, 0)),
	m_Forward(glm::vec3(0, 0, -1)),
	m_Up(glm::vec3(0, 1, 0)),
	m_Projection(glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane)) {}

Camera::Camera(const glm::vec3& p_pos,
	const float p_fov,
	const float p_aspect,
	const float p_zNear,
	const float p_zFar)
	:
	m_FOV(p_fov),
	m_AspectRatio(p_aspect),
	m_NearPlane(p_zNear),
	m_FarPlane(p_zFar),
	m_Pos(p_pos),
	m_Forward(glm::vec3(0, 0, -1)),
	m_Up(glm::vec3(0, 1, 0)),
	m_Projection(glm::perspective(p_fov, p_aspect, p_zNear, p_zFar))
{}


void Camera::MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2 & mouseVelocity)
{
	mDiff = glm::normalize(mDiff);
	if (glm::abs(mDiff.x) < 10 || glm::abs(mDiff.y) < 10)
	{
		RotateLocalY(-mDiff.x * camSpeed / 2 * glm::length(mouseVelocity));
		RotateLocalX(-mDiff.y * camSpeed / 2 * glm::length(mouseVelocity));
	}
}

//TODO: use a screen manager to get SCREENWIDTH dynamically!
Ray Camera::RayFromMouse(const double mouseX, const double mouseY) const
{
	const double x = (2.0 * mouseX) / static_cast<double>(SCREENWIDTH) - 1.0;
	const double y = 1.0 - (2.0 * mouseY) / static_cast<double>(SCREENHEIGHT);
	const double z = 1.0;
	const glm::mat4 proj_mat = GetProjectionMatrix();
	const glm::mat4 view_matrix = GetViewMatrix();

	//normalized device coordinates [-1:1, -1:1, -1:1]
	const glm::vec3 ray_nds(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

	// clip space (4d homogenized) [-1:1, -1:1, -1:1, -1:1]
	const glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);

	// eye space [-x:x, -y:y, -z:z, -w:w]
	glm::vec4 ray_eye = glm::inverse(proj_mat) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

	// world space [-x:x, -y:y, -z:z, -w:w]
	glm::vec3 ray_world = glm::vec3(glm::inverse(view_matrix) * ray_eye);
	ray_world = glm::normalize(ray_world);

	Ray ray(this->PositionRead(), ray_world);

	return ray;
}

void Camera::LookAt(const glm::vec3& target)
{
	SetViewVector(normalize(m_Pos - target));
}

void Camera::SetCamera2(Camera* camera)
{
	m_cam2 = camera;
}

void Camera::SetMainCamera(Camera* cam)
{
	m_mainCam = cam;
}

Camera* Camera::GetMain()
{
	return m_mainCam;
}

Camera* Camera::GetCam2()
{
	return m_cam2;
}

//TODO: set screen width and other vars dynamically!
void Camera::SetOrthographic(float halfWidth, float halfHeight, float _near, float _far)
{
	m_IsOrthographic = true;
	//float H = static_cast<float>(SCREENWIDTH) / 8.0f;
	m_NearPlane = _near;
	m_FarPlane = _far;
	m_Projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_NearPlane, m_FarPlane);
	//m_Projection = glm::ortho( 0.f,  halfWidth,  0.f, halfHeight, m_NearPlane , m_FarPlane    );
}

//float Camera::GetFrustumWidth()
//{
//	return 
//}

glm::mat4 Camera::GetViewProjectionMatrix() const
{
	//return m_Projection * glm::lookAt(m_Pos, m_Pos + m_Forward, m_Up);
	return GetProjectionMatrix() * GetViewMatrix();
}

std::pair<bool, Model*> Camera::MousePick(double MouseX, double MouseY) const
{
	const Ray ray = RayFromMouse(MouseX, MouseY);
	//glm::vec3 origin = GetPosition();
	std::vector < std::pair<Model*, float>> hits;
	float t_min = std::numeric_limits<float>::infinity();
	Model* m_current = nullptr;
	bool totalHit = false;

	for (std::shared_ptr<Model> mdl : EntityManager::GetEntities())
	{
		float tCurrent = -1111;
		bool hit = mdl->m_meshes[0].m_aabb.IntersectAABB(ray, tCurrent);
		///bool hit = intersectAABB(origin, dir, tCurrent, mdl->meshes[0].m_aabb);
		totalHit |= hit;
		if (hit && tCurrent < t_min)
		{
			m_current = mdl.get();
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
	return glm::lookAt(m_Pos, m_Pos + m_Forward, m_Up + glm::vec3(1,1,1) * 0.0001f);
}

//TODO: add this?
//void Camera::RecalcViewMatrix()
//{
//
//}

void Camera::Roll(const float angle)
{
	const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, m_Forward);
	//m_Forward = vec3(normalize(rotation * vec4(m_Up, 0.0)));

	m_Up = glm::vec3(normalize(rotation * glm::vec4(m_Up, 0.0)));
}

float& Camera::GetFieldOfView()  
{
	return m_FOV;
}

inline glm::mat4 Camera::GetProjectionMatrix() const
{
	return m_Projection;
}

void Camera::SetPerspective(const glm::vec3 & pos, float fov, float aspect, float zNear, float zFar)
{
	m_IsOrthographic = false;
	m_Projection = glm::perspective(fov, aspect, zNear, zFar);
}


//yaw 
inline void Camera::RotateLocalX(const float angle)
{
	const glm::vec3 right = glm::normalize(glm::cross(m_Up, m_Forward));
	const glm::mat4 rot = glm::rotate(glm::mat4(1), angle, right);

	m_Forward = glm::vec3(glm::normalize(rot * glm::vec4(m_Forward, 0.0)));
	m_Up = glm::normalize(glm::cross(m_Forward, right));
}


//rotate around the y axis, its own up vector
inline void Camera::RotateLocalY(const float angle)
{
	const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, m_Up);
	m_Forward = glm::vec3(glm::normalize(rotation * glm::vec4(m_Forward, 0.0)));

	m_Up = glm::vec3(glm::normalize(rotation * glm::vec4(m_Up, 0.0)));
}

void Camera::SetAspectRatio(const float aspectRatio)
{
	m_AspectRatio = aspectRatio;
	RecalcProjection();
}

void Camera::SetViewVector(const glm::vec3& view)
{
	m_Forward = view;
}

void Camera::SetUpVector(const glm::vec3& up)
{
	m_Up = up;
}

glm::vec3& Camera::GetPosition()
{
	return m_Pos;
}

glm::vec3 Camera::PositionRead() const
{
	return m_Pos;
}

float Camera::GetAspectRatio() const
{
	return m_AspectRatio;
}

glm::vec3 Camera::GetForwardVector() const
{
	return m_Forward;
}

float& Camera::GetNearPlaneDist()  
{
	return  m_NearPlane;
}

float& Camera::GetFarPlaneDist()  
{
	return m_FarPlane;
}

glm::vec3 Camera::GetUpVector() const
{
	return m_Up;
}

void Camera::RecalcProjection()
{
	m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}

