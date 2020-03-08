#include "precomp.h"
#include <glm/gtc/matrix_access.hpp>
#include "Model.h"
#include "Rendering/GPUShader.h"
#include "GameObject/Camera.h"
#include "GameObject/EntityManager.h"
#include "GameObject/Components/AABB.h"
#include "GameObject/Components/mesh.h"
#include "Geometry/Ray.h"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "Rendering/ShaderManager.h"

class Model;

//from Jim arvo 1990, gpu gems 
void AABB::UpdateArvo(const glm::mat4& m, const AABB& orig)
{
	const glm::vec3 t = glm::column(m, 3);
	for (int i = 0; i < 3; i++)
	{
		m_min.v[i] = t[i];
		m_max.v[i] = t[i];

		for (int j = 0; j < 3; j++)
		{
			float e = m[j][i] * orig.m_min.v[j];
			float f = m[j][i] * orig.m_max.v[j];
			m_min.v[i] += e < f ? e : f;
			m_max.v[i] += e < f ? f : e;
		}
	}
}

void AABB::Draw(const Camera& camera, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f })
{
	const glm::mat4 model = GetTransform();
	const glm::mat4 view = camera.GetViewMatrix();
	const glm::mat4 projection = camera.GetProjectionMatrix();

	Model* wirecube = nullptr;
	for (auto& e : EntityManager::GetEntities())
		if (e->name == "wirecube") wirecube = e; //TODO: FIX GETTING WIRECUBE! 

	if (wirecube == nullptr)
	{
		std::cout << "wirecube not initialized!\n";
		return;
	}

	wirecube->model = model;
	GPUShader& shader = ShaderManager::GetShader("AABB_single");
	shader.Bind();
	//shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);
	shader.SetUniform4f("u_color", color.x,color.y,color.z,color.w);

	Mesh& mesh = wirecube->getMesh(0);

	glBindVertexArray(mesh.GetVAO());
	if (!mesh.indices.empty())
		glDrawElements(GL_TRIANGLES,
			static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(mesh.GetElemDrawType(), 0, mesh.GetVertexCount()); //plane!

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	//wirecube->Draw(camera);


}

bool AABB::IntersectAABB(const Ray& ray, float& tCurrent) const
{
	const glm::vec3& direction = ray.m_direction;
	const glm::vec3& origin = ray.m_origin;

	const glm::vec3 d = glm::vec3(1.0f / direction.x,
		1.0f / direction.y,
		1.0f / direction.z);

	const float t = 99999.0f;
	const float tx1 = ( m_min.v.x - origin.x) * d.x;
	const float tx2 = ( m_max.v.x - origin.x) * d.x;
					    
	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	const float ty1 = ( m_min.v.y - origin.y) * d.y;
	const float ty2 = ( m_max.v.y - origin.y) * d.y;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	const float tz1 = ( m_min.v.z - origin.z) * d.z;
	const float tz2 = ( m_max.v.z - origin.z) * d.z;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));
	tCurrent = tmin;
	return tmax >= std::max(0.0f, tmin) && tmin < t;
}
