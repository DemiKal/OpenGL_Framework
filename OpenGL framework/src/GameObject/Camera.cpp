#include "precomp.h"
Camera* Camera::m_mainCam;

Camera::Camera() : fov(70), aspectRatio((float)SCREENWIDTH / (float)SCREENHEIGHT),
pos(glm::vec3(0, 0, 0)), forward(glm::vec3(0, 0, -1)),
up(glm::vec3(0, 1, 0)), projection(glm::perspective(70.f, aspectRatio, 0.1f, 200.0f)) {}

Camera::Camera(const glm::vec3& p_pos, float p_fov, float p_aspect, float p_zNear, float p_zFar)
	:
	fov(p_fov), aspectRatio(p_aspect), pos(p_pos), forward(glm::vec3(0, 0, -1)),
	up(glm::vec3(0, 1, 0)), projection(glm::perspective(p_fov, p_aspect, p_zNear, p_zFar))
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
glm::vec3 Camera::RayFromMouse(float mouseX, float mouseY) const
{
	float x = (2.0f * mouseX) / SCREENWIDTH - 1.0f; //TODO: FIX WITH DYNAMIC SCREENWIDTH!
	float y = 1.0f - (2.0f * mouseY) / SCREENHEIGHT;
	float z = 1.0f;
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

	return ray_world;
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

void Camera::meme(Cube & cyb)
{
	std::string s = cyb.Name();
}

bool intersectAABB(const glm::vec3 & origin, const glm::vec3 & direction, float& tCurrent, const AABB & aabb)
{
	const glm::vec3 d = glm::vec3(1.0f / direction.x,
		1.0f / direction.y,
		1.0f / direction.z);

	const float t = 99999.0f;
	const float tx1 = (aabb.m_min.v.x - origin.x) * d.x;
	const float tx2 = (aabb.m_max.v.x - origin.x) * d.x;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	const float ty1 = (aabb.m_min.v.y - origin.y) * d.y;
	const float ty2 = (aabb.m_max.v.y - origin.y) * d.y;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	const float tz1 = (aabb.m_min.v.z - origin.z) * d.z;
	const float tz2 = (aabb.m_max.v.z - origin.z) * d.z;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));
	tCurrent = tmin;
	return tmax >= std::max(0.0f, tmin) && tmin < t;
}

std::pair<bool, Model*> Camera::MousePick(double MouseX, double MouseY)
{
	glm::vec3 dir = RayFromMouse(MouseX, MouseY);
	glm::vec3 origin = GetPosition();
	std::vector < std::pair<Model*, float>> hits;
	float t_min = 99999999;
	Model* m_current = nullptr;
	bool totalHit = false;
	for (Model* mdl : EntityManager::GetEntities())
	{
		float tCurrent = -1111;
		bool hit = intersectAABB(origin, dir, tCurrent, mdl->meshes[0].m_aabb);
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

void Camera::CheckMouseHover(const float mX, const float mY, const Cube & cube)
{
	auto tris = cube.GetMeshTriangles();
	const glm::vec3  campos = GetPosition();
	const   glm::vec3 dir = RayFromMouse(mX, mY);

	bool cubeIntersect = false;
	for (const auto& tri : tris)
	{
		glm::vec2 baryCoords{ 0,0 };
		float t;

		const bool intersect = glm::intersectRayTriangle(campos,
			dir, tri.v1, (&tri)->v2, (&tri)->v3, baryCoords, t);

		if (intersect)
		{
			cubeIntersect = true;
			break;
		}
	}

	std::string name = "none";

	name = cubeIntersect ? cube.Name().c_str() : "none";

	ImGui::Text("Hovered obj: %s", name.c_str());
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

glm::vec3 Camera::GetUpVector() const
{
	return up;
}


