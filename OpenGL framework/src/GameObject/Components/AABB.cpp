#include "precomp.h"
#include "Model.h"
//#include "ShaderManager.h"
#include <glm/gtc/matrix_access.hpp>

class Model;

//from Jim arvo 1990, gpu gems 
void AABB::UpdateArvo(const glm::mat4& m, const AABB& orig)
{
	glm::vec3 t = glm::column(m, 3);
	for (int i = 0; i < 3; i++)
	{
		m_min.v[i] = t[i];
		m_max.v[i] = t[i];

		for (int j = 0; j < 3; j++)
		{
			float e = m[j][i] * orig.m_min.v[j];
			float f = m[j][i] * orig.m_max.v[j];
			//if (e < f) {
			//	m_min.v[i] += e;
			//	m_max.v[i] += f;
			//}
			//else {
			//	m_min.v[i] += f;
			//	m_max.v[i] += e;
			//}
			//int cond = e < f;
			m_min.v[i] += e < f ? e : f;
			m_max.v[i] += e < f ? f : e;
		}
	}


}

void AABB::UpdateNew(const glm::mat4& transform, const AABB& original)
{
	const glm::vec3 right = glm::column(transform, 0);
	const glm::vec3 up = glm::column(transform, 1);
	const glm::vec3 backwards = glm::column(transform, 2);
	const glm::vec3 translation = glm::column(transform, 3);
	//
	const  glm::vec3  xa = right * original.m_min.v.x;
	const  glm::vec3  xb = right * original.m_max.v.x;

	const  glm::vec3  ya = up * original.m_min.v.y;
	const  glm::vec3  yb = up * original.m_max.v.y;

	const  glm::vec3  za = backwards * original.m_min.v.z;
	const  glm::vec3  zb = backwards * original.m_max.v.z;
	//
	//
	const glm::vec3 minX = minimize(xa, xb);
	const glm::vec3 minY = minimize(ya, yb);
	const glm::vec3 minZ = minimize(za, zb);
	const glm::vec3 min = minX + minY + minZ + translation;
	//
	const glm::vec3 maxX = maximize(xa, xb);
	const glm::vec3 maxY = maximize(ya, yb);
	const glm::vec3 maxZ = maximize(za, zb);
	const glm::vec3 max = maxX + maxY + maxZ + translation;

	m_min.v = min;
	m_max.v = max;
}

void AABB::Draw(const Camera& camera)
{
	glm::mat4 model = GetTransform();
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = camera.GetProjectionMatrix();

	Model* wirecube = EntityManager::GetEntities()[2]; //TODO: FIX GETTING WIRECUBE! 
	wirecube->model = model;
	GPUShader& shader = wirecube->GetShader();
	shader.Bind();
	shader.SetVec4f("u_color", glm::vec4(1.0f, 0.75f, 0.5f, 1.0f));
	shader.SetUniformMat4f("model", model);
	shader.SetUniformMat4f("view", view);
	shader.SetUniformMat4f("projection", projection);

	MeshNew& mesh = wirecube->getMesh(0);

	glBindVertexArray(mesh.GetVAO());
	if (!mesh.indices.empty())
		glDrawElements(GL_TRIANGLES,
			static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
	else glDrawArrays(mesh.GetElemDrawType(), 0, mesh.GetVertexCount()); //plane!

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	//wirecube->Draw(camera);


}
