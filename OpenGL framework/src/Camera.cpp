#include "precomp.h"


//TODO: set this in a class globally accessible
static const int SCREENWIDTH = 1280;
static const int SCREENHEIGHT = 720;


Camera::Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar)
{
	this->pos = pos;
	this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->projection = glm::perspective(fov, aspect, zNear, zFar);

	this->aspectRatio = aspect;
	this->fov = fov;
}

void Camera::MoveCameraMouse(glm::vec2 mDiff, float camSpeed, glm::vec2& mvelo)
{
	mDiff = glm::normalize(mDiff);
	if (glm::abs(mDiff.x) < 10 || glm::abs(mDiff.y) < 10)
	{
		RotateYlocal(-mDiff.x * camSpeed / 2 * glm::length(mvelo));
		RotateXlocal(-mDiff.y * camSpeed / 2 * glm::length(mvelo));
	}
}

glm::vec3 Camera::RayFromMouse(float mouseX, float mouseY) const
{
	float x = (2.0f * mouseX) / SCREENWIDTH - 1.0f;
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

void Camera::SetOrthographic()
{
	this->projection = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -10.0f, 100.0f);
}

void Camera::meme(Cube& cyb)
{
	std::string s = cyb.Name();
}

void Camera::CheckMouseHover(double mX, double mY, Cube& cube)
{
	auto tris = cube.GetMeshTriangles();
	glm::vec3 const campos = *Position();
	auto  dir = RayFromMouse(mX, mY);

	bool cubeIntersect = false;
	for (const auto tri : tris)
	{
		glm::vec2 baryCoords{ 0,0 };
		float t;

		const bool intersect = glm::intersectRayTriangle(campos,
			dir, (&tri)->v1, (&tri)->v2, (&tri)->v3, baryCoords, t);

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


