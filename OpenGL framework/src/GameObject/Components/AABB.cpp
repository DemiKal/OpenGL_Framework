#include "precomp.h"
#include "Model.h"
//#include "ShaderManager.h"


class Model;

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
