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
			m_min.v[i] += e < f ? e : f;
			m_max.v[i] += e < f ? f : e;
		}
	}
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
