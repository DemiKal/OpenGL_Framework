#include "precomp.h"
#include <glm/gtc/matrix_access.hpp>
#include "Model.h"
#include "Rendering/Shader.h"
#include "GameObject/Camera.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/AABB.h"
#include "Geometry/Ray.h"
#include "Rendering/ShaderManager.h"

class Model;

//from Jim arvo 1990, gpu gems 
void AABB::UpdateArvo(const glm::mat4& m, const AABB& orig)
{
	const glm::vec3 t = glm::column(m, 3);
	for (int i = 0; i < 3; i++)
	{
		min[i] = t[i];
		max[i] = t[i];

		for (int j = 0; j < 3; j++)
		{
			float e = m[j][i] * orig.min[j];
			float f = m[j][i] * orig.max[j];
			min[i] += e < f ? e : f;
			max[i] += e < f ? f : e;
		}
	}
}





inline glm::vec3 AABB::Minimize(const glm::vec3& minA, const glm::vec3& minB) const
{
	return{ std::min(minA.x, minB.x),std::min(minA.y, minB.y),std::min(minA.z, minB.z) };
}

inline glm::vec3 AABB::Maximize(const glm::vec3& maxA, const glm::vec3& maxB) const
{
	return { std::max(maxA.x, maxB.x),std::max(maxA.y, maxB.y),std::max(maxA.z, maxB.z) };
}

 
inline std::vector<glm::vec4> AABB::GetVerticesLocal() const
{
	return
	{
		//front plane
		{min.x, min.y, max.z, 1.0f}, {max.x, min.y, max.z, 1.0f},
		{max.x, max.y, max.z, 1.0f}, {min.x, max.y, max.z, 1.0f},

		{min.x, min.y, min.z, 1.0f}, {max.x, min.y, min.z, 1.0f},
		{max.x, max.y, min.z, 1.0f}, {min.x, max.y, min.z, 1.0f},
	};
}

void AABB::RecalcBounds(const glm::mat4& transform, const AABB& original)
{
	const std::vector<glm::vec4> verts_local = original.GetVerticesLocal();
	std::vector<glm::vec3> verts_world;
	std::transform(verts_local.begin(), verts_local.end(),
		std::back_inserter(verts_world),
		[transform](const glm::vec4& v) { return glm::vec3(transform * v); });

	CalcBounds(verts_world);
}

void AABB::CalcBounds(const std::vector<glm::vec3>& posVector)
{
	glm::vec3 vmin(glm::vec3((float)std::numeric_limits<float>::infinity()));
	glm::vec3 vmax(glm::vec3((float)-std::numeric_limits<float>::infinity()));

	for (const glm::vec3& v : posVector)
	{
		vmin = Minimize(vmin, v);
		vmax = Maximize(vmax, v);
	}

	min = {vmin};
	max = {vmax};
}

glm::mat4 AABB::GetTransform() const
{
	const glm::vec3 scale = max - min;
	const glm::vec3 center = 0.5f * (max + min);

	return glm::scale(glm::translate(glm::mat4(1.0f), center), scale);
}

void AABB::Update(const glm::mat4& transform, const AABB& original)
{
	RecalcBounds(transform, original);
}

void AABB::Draw(const Camera& camera, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }) const
{
	const glm::mat4 model = GetTransform();
	const glm::mat4 view = camera.GetViewMatrix();
	const glm::mat4 projection = camera.GetProjectionMatrix();

	Shader& shader = ShaderManager::GetShader("AABB_single");
	shader.Bind();
	//shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);
	shader.SetVec4f("u_color", color);

	throw std::exception("AABB draw NOT YET IMPLEMENTED!"); //TODO: fix with new renderer.
	//GLCall(glBindVertexArray(mesh.GetVAO()));
	//if (mesh.indices.empty())
	//	GLCall(glDrawArrays(	mesh.GetElemDrawType(), 0, static_cast<GLsizei>(mesh.GetVertexCount()));)
	//else GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);)
	// 
	//
	//GLCall(glBindVertexArray(0));
	//GLCall(glActiveTexture(GL_TEXTURE0));

}

bool AABB::IntersectAABB(const Ray& ray, float& tCurrent) const
{
	const glm::vec3& direction = ray.m_direction;
	const glm::vec3& origin = ray.m_origin;

	const glm::vec3 d = glm::vec3(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);

	constexpr float t = std::numeric_limits<float>::infinity();
	const float tx1 = (min.x - origin.x) * d.x;
	const float tx2 = (max.x - origin.x) * d.x;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	const float ty1 = (min.y - origin.y) * d.y;
	const float ty2 = (max.y - origin.y) * d.y;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	const float tz1 = (min.z - origin.z) * d.z;
	const float tz2 = (max.z - origin.z) * d.z;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));
	tCurrent = tmin;
	return tmax >= std::max(0.0f, tmin) && tmin < t;
}
